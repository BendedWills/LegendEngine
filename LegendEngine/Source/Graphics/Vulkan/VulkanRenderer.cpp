#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Graphics/Vulkan/VertexBuffer.hpp>
#include <LegendEngine/Application3D.hpp>

#include <CompiledResources/solid.vert.spv.h>
#include <CompiledResources/solid.frag.spv.h>

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
    if (!pRenderer->RecreateSwapchain(event.GetNewWidth(), event.GetNewHeight()))
    {
        std::cout << "Failed to recreate swapchain!" << std::endl;
    }
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

bool VulkanRenderer::Reload()
{
    Tether::IWindow* pWindow = pApplication->GetWindow();
    return RecreateSwapchain(pWindow->GetWidth(), pWindow->GetHeight());
}

bool VulkanRenderer::OnRendererInit()
{
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

void VulkanRenderer::OnSceneChange(Scene3D* pScene)
{
    
}

void VulkanRenderer::OnSceneObjectAdd(Scene3D* pScene, 
    Object3d::Object* pObject)
{

}

void VulkanRenderer::OnSceneObjectRemove(Scene3D* pScene, 
    Object3d::Object* pObject)
{

}

void VulkanRenderer::OnSceneRemove(Scene3D* pScene)
{

}

void VulkanRenderer::OnDefaultObjectAdd(Scene3D* pScene, 
    Object3d::Object* pObject)
{

}

void VulkanRenderer::OnDefaultObjectRemove(Scene3D* pScene, 
    Object3d::Object* pObject)
{

}

bool VulkanRenderer::OnRenderFrame()
{
    return DrawFrame();
}

void VulkanRenderer::OnRendererDispose()
{
    device.WaitIdle();

    DisposeSwapchain();

    for (uint64_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device.Get(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device.Get(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device.Get(), inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(device.Get(), commandPool, nullptr);

    for (LegendEngine::VertexBuffer* buffer : vertexBuffers)
    {
        std::stringstream ss;
        ss << "Disposed VertexBuffer (" << (uint64_t)buffer << ")";

        LEGENDENGINE_OBJECT_LOG(pApplication, "VulkanRenderer", ss.str(), 
            LogType::DEBUG);
        
        buffer->Dispose();
    }

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

VkSurfaceFormatKHR VulkanRenderer::ChooseSurfaceFormat(Vulkan::SwapchainDetails details) 
{
    for (const auto& availableFormat : details.formats)
        if (availableFormat.format == VK_FORMAT_R64G64B64_UINT)
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
    vertexModule.Init(&device);
    fragmentModule.Init(&device);

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
    VkPipelineLayoutCreateInfo pipelineLayoutDesc{};
    pipelineLayoutDesc.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutDesc.setLayoutCount = 0; 
    pipelineLayoutDesc.pushConstantRangeCount = 0; 
    
    if (vkCreatePipelineLayout(device.Get(), &pipelineLayoutDesc, 
        nullptr, &pipelineLayout) != VK_SUCCESS)
        return false;
    
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;
    
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkExtent2D swapchainExtent = swapchain.GetExtent();

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

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    
    VkPipelineMultisampleStateCreateInfo multisampleState{};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.sampleShadingEnable = VK_FALSE;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleState.minSampleShading = 0.0f; 
    multisampleState.pSampleMask = nullptr; 
    multisampleState.alphaToCoverageEnable = VK_FALSE; 
    multisampleState.alphaToOneEnable = VK_FALSE; 

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO; 
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; 
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; 
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; 
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; 
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; 

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; 
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    // Oh, yes, cool thing about Vulkan, you can actually have multiple shader
    // stages for one shader module. That means that you can have a VSMain and
    // a PSMain in one shader module (aka a glsl file in this case).
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

    VkPipelineShaderStageCreateInfo stages[] =
    {
        vertexStage, fragmentStage
    };

    VkGraphicsPipelineCreateInfo pipelineDesc{};
    pipelineDesc.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineDesc.stageCount = 2;
    pipelineDesc.pStages = stages;
    pipelineDesc.pVertexInputState = &vertexInputInfo;
    pipelineDesc.pInputAssemblyState = &inputAssembly;
    pipelineDesc.pViewportState = &viewportState;
    pipelineDesc.pRasterizationState = &rasterizer;
    pipelineDesc.pMultisampleState = &multisampleState;
    pipelineDesc.pColorBlendState = &colorBlending;
    pipelineDesc.layout = pipelineLayout;
    pipelineDesc.renderPass = renderPass;
    pipelineDesc.subpass = 0;
    pipelineDesc.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device.Get(), VK_NULL_HANDLE,
        1, &pipelineDesc, nullptr, &pipeline) != VK_SUCCESS)
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
    poolInfo.queueFamilyIndex = indices.graphicsFamilyIndex;
    
    return vkCreateCommandPool(device.Get(), &poolInfo, nullptr, &commandPool)
        == VK_SUCCESS;
}

bool VulkanRenderer::InitCommandBuffers()
{
    VkExtent2D swapchainExtent = swapchain.GetExtent();

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
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
            return false;
        
        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = framebuffers[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchainExtent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, 
            VK_SUBPASS_CONTENTS_INLINE);
        {
            vkCmdBindPipeline(commandBuffers[i], 
                VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

            vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
        }
        vkCmdEndRenderPass(commandBuffers[i]);
        
        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
            return false;
    }
    
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

bool VulkanRenderer::DrawFrame()
{
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

bool VulkanRenderer::RecreateSwapchain(uint64_t width, uint64_t height)
{
    if (width == 0 || height == 0)
        return true;
    
    std::stringstream ss;
    ss << "Recreating swapchain (Window resize) ";
    ss << "(newWidth = " << width << ", newHeight = " << height << ")";

    LEGENDENGINE_OBJECT_LOG(pApplication, "VulkanRenderer", ss.str(), 
        LogType::DEBUG);
    
    // The device might still have work. Wait for it to finish before 
    // recreating the swapchain.
    device.WaitIdle();

    DisposeSwapchain();
    if (!InitSwapchain(width, height)
        || !InitRenderPass()
        || !InitShaders()
        || !InitPipeline()
        || !InitFramebuffers()
        || !InitCommandBuffers())
        return false;

    imagesInFlight.resize(swapchainImages.size(), VK_NULL_HANDLE);
    return true;
}

void VulkanRenderer::DisposeSwapchain()
{
    for (VkFramebuffer framebuffer : framebuffers)
        vkDestroyFramebuffer(device.Get(), framebuffer, nullptr);
    
    vkFreeCommandBuffers(device.Get(), commandPool, commandBuffers.size(), 
        commandBuffers.data());
    
    vkDestroyPipeline(device.Get(), pipeline, nullptr);
    vkDestroyPipelineLayout(device.Get(), pipelineLayout, nullptr);
    vkDestroyRenderPass(device.Get(), renderPass, nullptr);

    for (VkImageView imageView : swapchainImageViews)
        vkDestroyImageView(device.Get(), imageView, nullptr);

    swapchain.Dispose();
}

#endif // VULKAN_API