#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Graphics/Vulkan/VertexBuffer.hpp>
#include <LegendEngine/Graphics/Vulkan/Shader.hpp>
#include <LegendEngine/Application.hpp>

#include <CompiledAssets/solid.vert.spv.h>
#include <CompiledAssets/solid.frag.spv.h>

#include <set>

using namespace LegendEngine::Vulkan;

VulkanRenderer::EventHandler::EventHandler(VulkanRenderer* pRenderer)
{
    this->pRenderer = pRenderer;
}

void VulkanRenderer::EventHandler::OnWindowResize(
    Tether::Events::WindowResizeEvent event
)
{
    this->pRenderer->shouldRecreateSwapchain = true;
}

void VulkanRenderer::SetVSyncEnabled(bool vsync)
{
    this->enableVsync = vsync;

    Reload();
}

bool VulkanRenderer::CreateVertexBuffer(
    Ref<LegendEngine::VertexBuffer>* buffer)
{
    if (!buffer)
    {
        pApplication->Log(
            "Creating vertex buffer: Buffer is nullptr. Returning.",
            LogType::WARN);
        return false;
    }

    *buffer = RefTools::Create<VertexBuffer>(this);
    
    return true;
}

bool VulkanRenderer::CreateShader(
    Ref<LegendEngine::Shader>* shader)
{
    if (!shader)
    {
        pApplication->Log(
            "Creating shader: Shader is nullptr. Returning.",
            LogType::WARN);
        return false;
    }

    *shader = RefTools::Create<Shader>(this);

    return true;
}

bool VulkanRenderer::Reload()
{
    return RecreateSwapchain();
}

bool VulkanRenderer::OnRendererInit()
{
    timer.Start();

    pApplication = GetApplication();
    pInstance = pApplication->GetVulkanInstance();

    if (!pApplication->IsVulkanInitialized())
    {
        pApplication->Log("Vulkan was not initialized for the application",
            LogType::ERROR);
        return false;
    }

    pApplication->GetWindow()->AddEventHandler(&eventHandler, 
        Tether::Events::EventType::WINDOW_RESIZE);

    if (!surface.Init(pInstance, GetApplication()->GetWindow()))
        return false;
    
    if (!PickDevice(&physicalDevice, &surface))
        return false;
    
    indices = pInstance->FindQueueFamilies(physicalDevice, &surface);

    if (!InitDevice())
    {
        pApplication->Log("Failed to initialize device!", LogType::ERROR);
        return false;
    }

    if (!InitAllocator())
    {
        pApplication->Log("Failed to initialize allocator!", LogType::ERROR);
        return false;
    }

    if (!InitSwapchain(
        pApplication->GetWindow()->GetWidth(), 
        pApplication->GetWindow()->GetHeight()
    ))
    {
        pApplication->Log("Failed to initialize swapchain!", LogType::ERROR);
        return false;
    }

    if (!InitRenderPass())
    {
        pApplication->Log("Failed to initialize render pass!", LogType::ERROR);
        return false;
    }

    if (!InitShaders())
    {
        pApplication->Log("Failed to initialize shaders!", LogType::ERROR);
        return false;
    }

    if (!InitPipeline())
    {
        pApplication->Log("Failed to initialize pipeline!", LogType::ERROR);
        return false;
    }

    if (!InitFramebuffers())
    {
        pApplication->Log("Failed to initialize framebuffers!", LogType::ERROR);
        return false;
    }

    if (!InitCommandPool())
    {
        pApplication->Log("Failed to initialize command pool!", LogType::ERROR);
        return false;
    }

    if (!InitCommandBuffers())
    {
        pApplication->Log("Failed to initialize command buffers!", LogType::ERROR);
        return false;
    }

    if (!InitSyncObjects())
    {
        pApplication->Log("Failed to initialize sync objects!", LogType::ERROR);
        return false;
    }
    
    return true;
}

void VulkanRenderer::OnSceneChange(Scene* pScene)
{
    RecreateCommandBuffers();
}

void VulkanRenderer::OnSceneObjectAdd(Scene* pScene, 
    Objects::Object* pObject)
{
    RecreateCommandBuffers();
}

void VulkanRenderer::OnSceneObjectRemove(Scene* pScene, 
    Objects::Object* pObject)
{
    RecreateCommandBuffers();
}

void VulkanRenderer::OnSceneRemove(Scene* pScene)
{
    RecreateCommandBuffers();
}

void VulkanRenderer::OnDefaultObjectAdd(Scene* pScene, 
    Objects::Object* pObject)
{
    RecreateCommandBuffers();
}

void VulkanRenderer::OnDefaultObjectRemove(Scene* pScene, 
    Objects::Object* pObject)
{
    RecreateCommandBuffers();
}

void VulkanRenderer::OnSceneObjectComponentAdd(
    Scene* pScene,
    Objects::Object* pObject,
    const std::string& typeName,
    Objects::Components::Component* pComponent
)
{
    RecreateCommandBuffers();
}

void VulkanRenderer::OnSceneObjectComponentRemove(
    Scene* pScene,
    Objects::Object* pObject,
    const std::string& typeName,
    Objects::Components::Component* pComponent
)
{
    RecreateCommandBuffers();
}

bool VulkanRenderer::OnRenderFrame()
{
    return DrawFrame();
}

void VulkanRenderer::OnRendererDispose()
{
    device.WaitIdle();

    // Shaders are removed as they are disposed, so an original copy is
    // required.
    std::vector<LegendEngine::Shader*> shadersOriginal(shaders);
    for (uint64_t i2 = 0; i2 < shadersOriginal.size(); i2++)
    {
        LegendEngine::Shader* shader = shadersOriginal[i2];
        shader->Dispose();
    }
    
    DisposeSwapchain();

    testUniform.Dispose();
    shaderProgram.Dispose();
    vkDestroyRenderPass(device.Get(), renderPass, nullptr);
    
    for (uint64_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device.Get(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device.Get(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device.Get(), inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(device.Get(), commandPool, nullptr);

    // Vertex Buffers are removed as they are disposed, so an original copy is
    // required.
    std::vector<LegendEngine::VertexBuffer*> vertexBuffersOriginal(vertexBuffers);
    for (uint64_t i2 = 0; i2 < vertexBuffersOriginal.size(); i2++)
    {
        LegendEngine::VertexBuffer* buffer = vertexBuffersOriginal[i2];
        buffer->Dispose();
    }

    vmaDestroyAllocator(allocator);

    device.Dispose();
    surface.Dispose();

    pApplication->GetWindow()->RemoveEventHandler(&eventHandler);
}

bool VulkanRenderer::PickDevice(VkPhysicalDevice* pDevice, 
    Vulkan::Surface* pSurface)
{
    uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(pInstance->Get(), &deviceCount, nullptr);
	if (deviceCount == 0)
		return false;
	
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(pInstance->Get(), &deviceCount, 
        devices.data());

	for (VkPhysicalDevice device : devices)
		if (IsDeviceSuitable(device, pSurface))
		{
			*pDevice = device;
			return true;
		}
	
	return false;
}

bool VulkanRenderer::IsDeviceSuitable(VkPhysicalDevice device, 
    Vulkan::Surface* pSurface)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	Vulkan::QueueFamilyIndices families = 
        pInstance->FindQueueFamilies(device, pSurface);

	bool extentionsSupported = pInstance->CheckDeviceExtentionSupport(device,
        deviceExtensions.data(), deviceExtensions.size());

	bool swapChainGood = false;
	if (extentionsSupported)
	{
		Vulkan::SwapchainDetails details = pInstance->QuerySwapchainSupport(device, 
            pSurface);
		swapChainGood = !details.formats.empty() 
			&& !details.presentModes.empty();
	}

	return 
		deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
		&& deviceFeatures.geometryShader
		&& families.hasGraphicsFamily
		&& families.hasPresentFamily
		&& swapChainGood
		&& extentionsSupported;
}

bool VulkanRenderer::InitDevice()
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
        indices.graphicsFamilyIndex,
        indices.presentFamilyIndex
    };
    
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures features{};

    if (!device.Init(
            pInstance, 
            physicalDevice, 
            queueCreateInfos.data(), 
            queueCreateInfos.size(), 
            features, 
            deviceExtensions.data(),
            deviceExtensions.size()
        ))
        return false;
    
    graphicsQueue = device.GetDeviceQueue(indices.graphicsFamilyIndex, 0);
    presentQueue = device.GetDeviceQueue(indices.presentFamilyIndex, 0);

    return true;
}

bool VulkanRenderer::InitAllocator()
{
    VmaVulkanFunctions funcs{};
    funcs.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    funcs.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;
    
    VmaAllocatorCreateInfo createInfo{};
    createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    createInfo.physicalDevice = physicalDevice;
    createInfo.device = device.Get();
    createInfo.instance = pApplication->GetVulkanInstance()->Get();
    createInfo.pVulkanFunctions = &funcs;
    
    return vmaCreateAllocator(&createInfo, &allocator) == VK_SUCCESS;
}

VkSurfaceFormatKHR VulkanRenderer::ChooseSurfaceFormat(Vulkan::SwapchainDetails details) 
{
    for (const auto& availableFormat : details.formats)
        if (availableFormat.format == VK_FORMAT_R32G32B32_UINT)
            return availableFormat;

    return details.formats[0];
}

bool VulkanRenderer::InitSwapchain(uint64_t width, uint64_t height)
{
    Vulkan::SwapchainDetails details = 
        pInstance->QuerySwapchainSupport(physicalDevice, &surface);
    VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(details);

    uint32_t imageCount = details.capabilities.minImageCount + 1;
    if (details.capabilities.maxImageCount > 0 && 
        imageCount > details.capabilities.maxImageCount)
        imageCount = details.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface.Get();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = swapchain.ChooseExtent(details.capabilities, 
        width, height);
	createInfo.imageArrayLayers = 1;

	// This will probably be changed later on to take in a parameter in this
	// function.
	// This parameter in the swapchain specifies what the images in the
	// swapchain are used for.
	// This may seem familiar if you have done computer graphics before,
	// as this is only the color attachment. There are others too, such as
	// the depth buffer.
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform = details.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = swapchain.ChoosePresentMode(details.presentModes,
        enableVsync);
	createInfo.clipped = true;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

    indices = pInstance->FindQueueFamilies(physicalDevice, &surface);
    if (indices.graphicsFamilyIndex != indices.presentFamilyIndex)
    {
        if (!indices.hasPresentFamily)
			return false;
			
		uint32_t queueFamilyIndices[] = 
		{
			indices.graphicsFamilyIndex, 
			indices.presentFamilyIndex
		};

		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    if (!swapchain.Init(&surface, &device, details, 
        &createInfo))
        return false;
    
    swapchainImages = swapchain.GetImages();
    if (!swapchain.GetImageViews(&swapchainImageViews))
        return false;
    
    return true;
}

bool VulkanRenderer::InitShaders()
{
    vertexModule.Init(&device, ShaderType::VERTEX);
    fragmentModule.Init(&device, ShaderType::FRAG);

    if (!vertexModule.FromSpirV(
        (uint32_t*)LegendEngine::Resources::solid_vert_spv,
        sizeof(LegendEngine::Resources::solid_vert_spv)
    ))
    {
        pApplication->Log("Vertex creation failed!", LogType::ERROR);
        return false;
    }
    
    if (!fragmentModule.FromSpirV(
        (uint32_t*)LegendEngine::Resources::solid_frag_spv,
        sizeof(LegendEngine::Resources::solid_frag_spv)
    ))
    {
        pApplication->Log("Fragment creation failed!", LogType::ERROR);
        return false;
    }
    
    return true;
}

bool VulkanRenderer::InitRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchain.GetImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentReference{};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentReference;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo desc{};
    desc.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    desc.attachmentCount = 1;
    desc.pAttachments = &colorAttachment;
    desc.subpassCount = 1;
    desc.pSubpasses = &subpass;
    desc.dependencyCount = 1;
    desc.pDependencies = &dependency;

    if (vkCreateRenderPass(device.Get(), &desc, nullptr, &renderPass)
        != VK_SUCCESS)
        return false;

    return true;
}


bool VulkanRenderer::InitPipeline()
{
    if (!testUniform.Init(this, sizeof(float), 0, VK_SHADER_STAGE_FRAGMENT_BIT,
        swapchain.GetImageCount()))
        return false;

	if (!greenUniform.Init(this, sizeof(float), 1, VK_SHADER_STAGE_FRAGMENT_BIT,
		swapchain.GetImageCount()))
		return false;

    VkPipelineShaderStageCreateInfo vertexStage{};
    vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStage.module = vertexModule.Get();
    vertexStage.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentStage{};
    fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStage.module = fragmentModule.Get();
    fragmentStage.pName = "main";

	UniformBuffer* uniforms[]
	{
		&testUniform,
        &greenUniform
	};

    VkPipelineShaderStageCreateInfo stages[] =
    {
        vertexStage, fragmentStage
    };

    VkDynamicState dynamicStates[] =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    PipelineInfo pipelineInfo{};
	pipelineInfo.stageCount = sizeof(stages) / sizeof(stages[0]);
	pipelineInfo.pStages = stages;
	pipelineInfo.pDynamicStates = dynamicStates;
    pipelineInfo.dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
    pipelineInfo.pDynamicStates = dynamicStates;
    pipelineInfo.uniformCount = sizeof(uniforms) / sizeof(uniforms[0]);
    pipelineInfo.ppUniforms = uniforms;
    pipelineInfo.images = swapchain.GetImageCount();

    if (!shaderProgram.Init(this, &pipelineInfo))
        return false;

    vertexModule.Dispose();
    fragmentModule.Dispose();

    return true;
}

bool VulkanRenderer::InitFramebuffers()
{
    VkExtent2D swapchainExtent = swapchain.GetExtent();
    uint64_t imageViewCount = swapchainImageViews.size();
    
    framebuffers.resize(imageViewCount);

    for (uint64_t i = 0; i < imageViewCount; i++)
    {
        VkFramebufferCreateInfo framebufferDesc{};
        framebufferDesc.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferDesc.renderPass = renderPass;
        framebufferDesc.attachmentCount = 1;
        framebufferDesc.pAttachments = &swapchainImageViews[i];
        framebufferDesc.width = swapchainExtent.width;
        framebufferDesc.height = swapchainExtent.height;
        framebufferDesc.layers = 1;

        if (vkCreateFramebuffer(device.Get(), &framebufferDesc, nullptr,
            &framebuffers[i]) != VK_SUCCESS)
            return false;
    }

    return true;
}

bool VulkanRenderer::InitCommandPool()
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = indices.graphicsFamilyIndex;
    
    return vkCreateCommandPool(device.Get(), &poolInfo, nullptr, &commandPool)
        == VK_SUCCESS;
}

bool VulkanRenderer::InitCommandBuffers()
{
    commandBuffers.resize(framebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(device.Get(), &allocInfo, 
        commandBuffers.data()) != VK_SUCCESS)
        return false;
    
    for (uint64_t i = 0; i < commandBuffers.size(); i++)
        PopulateCommandBuffer(commandBuffers[i], framebuffers[i], i);
    
    return true;
}

bool VulkanRenderer::InitSyncObjects()
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(swapchainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint64_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(device.Get(), &semaphoreInfo, nullptr, 
            &imageAvailableSemaphores[i]) != VK_SUCCESS)
            return false;
        if (vkCreateSemaphore(device.Get(), &semaphoreInfo, nullptr, 
            &renderFinishedSemaphores[i]) != VK_SUCCESS)
            return false;
        if (vkCreateFence(device.Get(), &fenceInfo, nullptr, 
            &inFlightFences[i]) != VK_SUCCESS)
            return false;
    }

    return true;
}

void VulkanRenderer::UpdateUniforms(uint64_t imageIndex)
{
    float test = (sin(timer.GetElapsedMillis() / 500.0f) + 1.0f) / 2;
    testUniform.UpdateBuffer(&test, imageIndex);

	float green = (sin(timer.GetElapsedMillis() / 1000.0f) + 1.0f) / 2;
	greenUniform.UpdateBuffer(&green, imageIndex);
}

bool VulkanRenderer::DrawFrame()
{
    if (shouldRecreateSwapchain)
        if (!RecreateSwapchain())
            pApplication->Log("Failed to recreate swapchain!", LogType::ERROR);

    // in flight frame = a frame that is being rendered while still rendering 
    // more frames

    // If this frame is still in flight, wait for it to finish rendering before
    // rendering another frame.
    vkWaitForFences(device.Get(), 1, &inFlightFences[currentFrame], VK_TRUE, 
        UINT64_MAX);
    
    // Get the next swapchain image. The swapchain has the minimum
    // amount of images plus one.
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device.Get(), 
        swapchain.Get(), UINT64_MAX, imageAvailableSemaphores[currentFrame], 
        VK_NULL_HANDLE, &imageIndex);
    // vkAcquireNextImageKHR might throw an error.
    // If it does throw an error, simply return true if it is suboptimal or
    // out of date.
    if (result != VK_SUCCESS)
        return result == VK_SUBOPTIMAL_KHR 
            || result == VK_ERROR_OUT_OF_DATE_KHR;

    UpdateUniforms(imageIndex);
    
    // If images are acquired out of order, or MAX_FRAMES_IN_FLIGHT is higher
    // than the number of swapchain images, we may start rendering to an
    // image that is in flight.
    // Check for that here.
    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
        vkWaitForFences(device.Get(), 1, &imagesInFlight[imageIndex], VK_TRUE,
            UINT64_MAX);
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    // Wait for the image to be available before rendering the frame and
    // signal the render finished semaphore once rendering is complete.
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    
    // The in flight fence for this frame must be reset before rendering.
    vkResetFences(device.Get(), 1, &inFlightFences[currentFrame]);
    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, 
        inFlightFences[currentFrame]) != VK_SUCCESS)
        return false;
    
    // Wait for the frame to be rendered until presenting
    // (hence the wait semaphores being the signal semaphores)
    VkSwapchainKHR swapchains[] = { swapchain.Get() };
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;

    if (vkQueuePresentKHR(presentQueue, &presentInfo) 
        != VK_SUCCESS)
        return true;
    
    // Increment the frame.
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    return true;
}

bool VulkanRenderer::RecreateSwapchain()
{
    LEGENDENGINE_OBJECT_LOG(pApplication, "VulkanRenderer", 
        "Recreating swapchain (Window resize)", 
        LogType::DEBUG);
    
    // The device might still have work. Wait for it to finish before 
    // recreating the swapchain.
    device.WaitIdle();

    Tether::IWindow* pWindow = pApplication->GetWindow();

    DisposeSwapchain();
    if (!InitSwapchain(
            pWindow->GetWidth(),
            pWindow->GetHeight()
        )
        || !InitFramebuffers()
        || !InitCommandBuffers())
        return false;
    
    imagesInFlight.resize(swapchainImages.size(), VK_NULL_HANDLE);

    shouldRecreateSwapchain = false;
    return true;
}

bool VulkanRenderer::RecreateCommandBuffers()
{
    LEGENDENGINE_ASSERT_INITIALIZED_RET(true);

    for (uint64_t i = 0; i < commandBuffers.size(); i++)
    {
        // Wait for all frames to finish rendering.
        // Command buffers cannot be reset during frame rendering.
        for (uint64_t i2 = 0; i2 < inFlightFences.size(); i2++)
            vkWaitForFences(device.Get(), 1, &inFlightFences[i2], true, UINT64_MAX);

        vkResetCommandBuffer(commandBuffers[i], 0);
        if (!PopulateCommandBuffer(commandBuffers[i], framebuffers[i], i))
            return false;
    }

    return true;
}

bool VulkanRenderer::PopulateCommandBuffer(VkCommandBuffer buffer,
    VkFramebuffer framebuffer, uint64_t commandBufferIndex)
{
    LEGENDENGINE_ASSERT_INITIALIZED_RET(true);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS)
        return false;

    VkExtent2D swapchainExtent = swapchain.GetExtent();
    
    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapchainExtent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapchainExtent.width;
    viewport.height = (float)swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = swapchainExtent.width;
    scissor.extent.height = swapchainExtent.height;

    vkCmdBeginRenderPass(buffer, &renderPassInfo,
        VK_SUBPASS_CONTENTS_INLINE);
    {
        vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, 
            shaderProgram.GetPipeline());

        vkCmdSetViewport(buffer, 0, 1, &viewport);
        vkCmdSetScissor(buffer, 0, 1, &scissor);

        using namespace Objects;
        using namespace Objects::Components;

        // This will probably be changed later on
        // Eventually, objects will have to be rendered in order of distance
        // from the camera.

        VkDescriptorSet descSets[] =
        {
			testUniform.GetDescriptorSets()[commandBufferIndex],
			greenUniform.GetDescriptorSets()[commandBufferIndex]
        };

        vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            shaderProgram.GetPipelineLayout(), 0, 2, descSets, 0, nullptr);

        // Default scene
        Scene* pDefault = pApplication->GetDefaultScene();
        std::vector<Object*>* objects = pDefault->GetObjects();
        for (uint64_t i = 0; i < objects->size(); i++)
        {
            Object* object = objects->at(i);

            // The object must have a mesh component
            MeshComponent* component = object->GetComponent<MeshComponent>();
            if (!component)
                continue;

            LegendEngine::VertexBuffer* pVertexBuffer =
                component->GetVertexBuffer();
            if (pVertexBuffer->GetType() != RealRenderingAPI::VULKAN)
                continue;

            VertexBuffer* pVkVertexBuffer = (VertexBuffer*)pVertexBuffer;

            VkBuffer vbuffers[] = { pVkVertexBuffer->vertexBuffer };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(buffer, 0, 1, vbuffers, offsets);

            vkCmdDraw(buffer, component->GetVertexCount(), 1, 0, 0);
        }

        // ActiveScene
        Scene* pActive = pApplication->GetActiveScene();
        if (pActive)
        {
            std::vector<Object*>* activeObjects = pActive->GetObjects();
            for (uint64_t i = 0; i < activeObjects->size(); i++)
            {
                Object* object = activeObjects->at(i);

                // The object must have a mesh component
                MeshComponent* component = object->GetComponent<MeshComponent>();
                if (!component)
                    continue;

                LegendEngine::VertexBuffer* pVertexBuffer =
                    component->GetVertexBuffer();
                if (pVertexBuffer->GetType() != RealRenderingAPI::VULKAN)
                    continue;

                VertexBuffer* pVkVertexBuffer = (VertexBuffer*)pVertexBuffer;

                VkBuffer vbuffers[] = { pVkVertexBuffer->vertexBuffer };
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers(buffer, 0, 1, vbuffers, offsets);

                vkCmdDraw(buffer, component->GetVertexCount(), 1, 0, 0);
            }
        }
    }
    vkCmdEndRenderPass(buffer);
    
    if (vkEndCommandBuffer(buffer) != VK_SUCCESS)
        return false;

    return true;
}

void VulkanRenderer::DisposeSwapchain()
{
    vkFreeCommandBuffers(device.Get(), commandPool, commandBuffers.size(), 
        commandBuffers.data());

    for (VkFramebuffer framebuffer : framebuffers)
        vkDestroyFramebuffer(device.Get(), framebuffer, nullptr);
    
    for (VkImageView imageView : swapchainImageViews)
        vkDestroyImageView(device.Get(), imageView, nullptr);

    swapchain.Dispose();
}

#endif // VULKAN_API