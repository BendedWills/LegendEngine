#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Application.hpp>
#include <LegendEngine/Context.hpp>

#include <CompiledAssets/solid.vert.spv.h>
#include <CompiledAssets/solid.frag.spv.h>

#include <set>
#include <cmath>

#ifdef VMA_VULKAN_VERSION
#undef VMA_VULKAN_VERSION
#endif

#define VMA_VULKAN_VERSION 1000000
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

using namespace LegendEngine::Vulkan;

void VulkanRenderer::SetVSyncEnabled(bool vsync)
{
	LEGENDENGINE_ASSERT_INITIALIZED();

	this->enableVsync = vsync;

	Reload();
}

bool VulkanRenderer::CreateShaderNative(Resources::Shader* shader)
{
	LEGENDENGINE_ASSERT_INITIALIZED_RET(false);

	if (!shader)
	{
		pApplication->Log(
			"Creating shader: Shader is nullptr. Returning.",
			LogType::WARN);
		return false;
	}

	shader->SetNative(RefTools::Create<ShaderNative>(this, shader));

	return true;
}

bool VulkanRenderer::CreateTexture2DNative(Resources::Texture2D* texture)
{
	LEGENDENGINE_ASSERT_INITIALIZED_RET(false);

	if (!texture)
	{
		pApplication->Log(
			"Creating texture native: Texture is nullptr. Returning.",
			LogType::WARN);
		return false;
	}

	texture->SetNative(RefTools::Create<Texture2DNative>(this, texture));

	return true;
}

bool VulkanRenderer::Reload()
{
	LEGENDENGINE_ASSERT_INITIALIZED_RET(false);
	return RecreateSwapchain();
}

bool VulkanRenderer::CreateImageView(VkImageView* pImageView, VkImage image,
	VkFormat format, VkImageViewType viewType)
{
	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.format = format;
	createInfo.viewType = viewType;
	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	return vkCreateImageView(device.Get(), &createInfo, nullptr, pImageView) 
		== VK_SUCCESS;
}

bool VulkanRenderer::BeginSingleUseCommandBuffer(VkCommandBuffer* pCommandBuffer)
{
	VkCommandBufferAllocateInfo cmdAllocInfo{};
	cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdAllocInfo.commandPool = commandPool;
	cmdAllocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(device.Get(), &cmdAllocInfo,
		pCommandBuffer) != VK_SUCCESS)
		return false;

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	// +1 good practice points
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(*pCommandBuffer, &beginInfo);
	
	return true;
}

bool VulkanRenderer::EndSingleUseCommandBuffer(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo,
		VK_NULL_HANDLE) != VK_SUCCESS)
	{
		vkFreeCommandBuffers(device.Get(), commandPool, 1, &commandBuffer);
		return false;
	}

	// Wait for the data to finish uploading
	vkQueueWaitIdle(graphicsQueue);

	// Free the command buffer
	vkFreeCommandBuffers(device.Get(), commandPool, 1, &commandBuffer);

	return true;
}

bool VulkanRenderer::CreateStagingBuffer(VkBuffer* pBuffer, VmaAllocation* pAllocation,
	VmaAllocationInfo* pAllocInfo, uint64_t size)
{
	VkBufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = size;
	createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	// Create the staging buffer
	return vmaCreateBuffer(allocator, &createInfo,
		&allocInfo, pBuffer, pAllocation, pAllocInfo) == VK_SUCCESS;
}

bool VulkanRenderer::ChangeImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer;
	if (!BeginSingleUseCommandBuffer(&commandBuffer))
		return false;
	
	VkImageMemoryBarrier memoryBarrier{};
	memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	memoryBarrier.oldLayout = oldLayout;
	memoryBarrier.newLayout = newLayout;
	memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.image = image;
	memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	memoryBarrier.subresourceRange.baseMipLevel = 0;
	memoryBarrier.subresourceRange.levelCount = 1;
	memoryBarrier.subresourceRange.baseArrayLayer = 0;
	memoryBarrier.subresourceRange.layerCount = 1;
	memoryBarrier.srcAccessMask = 0;
	memoryBarrier.dstAccessMask = 0;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED 
		&& newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
	{
		memoryBarrier.srcAccessMask = 0;
		memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL 
		&& newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
	{
		memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
		return false;

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &memoryBarrier
	);

	return EndSingleUseCommandBuffer(commandBuffer);
}

bool VulkanRenderer::CopyBufferToImage(VkBuffer buffer, VkImage image, uint64_t width,
	uint64_t height)
{
	VkCommandBuffer commandBuffer;
	if (!BeginSingleUseCommandBuffer(&commandBuffer))
		return false;

	VkBufferImageCopy imageCopy{};
	imageCopy.bufferOffset = 0;
	imageCopy.bufferRowLength = 0;
	imageCopy.bufferImageHeight = 0;
	imageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopy.imageSubresource.mipLevel = 0;
	imageCopy.imageSubresource.baseArrayLayer = 0;
	imageCopy.imageSubresource.layerCount = 1;
	imageCopy.imageOffset = { 0, 0, 0 };
	imageCopy.imageExtent.width = static_cast<uint32_t>(width);
	imageCopy.imageExtent.height = static_cast<uint32_t>(height);
	imageCopy.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&imageCopy
	);

	return EndSingleUseCommandBuffer(commandBuffer);
}

bool VulkanRenderer::OnRendererInit()
{
	timer.Set();

	pApplication = GetApplication();
	pInstance = Context::GetVulkanInstance();

	if (!Context::IsVulkanInitialized())
	{
		pApplication->Log("Vulkan was not initialized for the Context",
			LogType::ERROR);
		return false;
	}

	if (!surface.Init(pInstance, pApplication->pWindow))
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
		pApplication->pWindow->GetWidth(), 
		pApplication->pWindow->GetHeight()
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

	if (!InitUniforms())
	{
		pApplication->Log("Failed to initialize uniforms!", LogType::ERROR);
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
	shouldRecreateCommandBuffers = true;
}

void VulkanRenderer::OnSceneObjectAdd(Scene* pScene, 
	Objects::Object* pObject)
{
	shouldRecreateCommandBuffers = true;
}

void VulkanRenderer::OnSceneObjectRemove(Scene* pScene, 
	Objects::Object* pObject)
{
	shouldRecreateCommandBuffers = true;
}

void VulkanRenderer::OnSceneRemove(Scene* pScene)
{
	shouldRecreateCommandBuffers = true;
}

void VulkanRenderer::OnDefaultObjectAdd(Scene* pScene, 
	Objects::Object* pObject)
{
	shouldRecreateCommandBuffers = true;
}

void VulkanRenderer::OnDefaultObjectRemove(Scene* pScene, 
	Objects::Object* pObject)
{
	shouldRecreateCommandBuffers = true;
}

void VulkanRenderer::OnSceneObjectComponentAdd(
	Scene* pScene,
	Objects::Object* pObject,
	const std::string& typeName,
	Objects::Components::Component* pComponent
)
{
	shouldRecreateCommandBuffers = true;
}

void VulkanRenderer::OnSceneObjectComponentRemove(
	Scene* pScene,
	Objects::Object* pObject,
	const std::string& typeName,
	Objects::Components::Component* pComponent
)
{
	shouldRecreateCommandBuffers = true;
}

void VulkanRenderer::OnSceneObjectEnable(Scene* pScene, Objects::Object* pObject)
{
	shouldRecreateCommandBuffers = true;
}

void VulkanRenderer::OnSceneObjectDisable(Scene* pScene, Objects::Object* pObject)
{
	shouldRecreateCommandBuffers = true;
}

bool VulkanRenderer::OnRenderFrame()
{
	return DrawFrame();
}

void VulkanRenderer::OnRendererDispose()
{
	device.WaitIdle();
	
	DisposeSwapchain();

	shaderProgram.Dispose();

	vkDestroyDescriptorSetLayout(device.Get(), objectLayout, nullptr);
	vkDestroyDescriptorSetLayout(device.Get(), cameraLayout, nullptr);
	vkDestroyDescriptorSetLayout(device.Get(), materialLayout, nullptr);
	cameraUniform.Dispose();
	cameraManager.Dispose();

	vkDestroyRenderPass(device.Get(), renderPass, nullptr);
	
	for (uint64_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(device.Get(), renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device.Get(), imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device.Get(), inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(device.Get(), commandPool, nullptr);

	vmaDestroyAllocator(allocator);

	device.Dispose();
	surface.Dispose();
}

void VulkanRenderer::OnWindowResize()
{
	shouldRecreateSwapchain = true;
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
			static_cast<uint32_t>(queueCreateInfos.size()),
			features, 
			deviceExtensions.data(),
			static_cast<uint32_t>(deviceExtensions.size())
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
	createInfo.vulkanApiVersion = VK_API_VERSION_1_0;
	createInfo.physicalDevice = physicalDevice;
	createInfo.device = device.Get();
	createInfo.instance = pInstance->Get();
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

bool VulkanRenderer::InitUniforms()
{
	uint32_t swapchainImages = swapchain.GetImageCount();

	// Object set
	{
		VkDescriptorSetLayoutBinding binding{};
		binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutCreateInfo setInfo{};
		setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		setInfo.bindingCount = 1;
		setInfo.pBindings = &binding;

		if (vkCreateDescriptorSetLayout(device.Get(),
			&setInfo, nullptr, &objectLayout) != VK_SUCCESS)
			return false;
	}

	// Camera set
	{
		VkDescriptorSetLayoutBinding cameraSetBinding{};
		cameraSetBinding.binding = 0;
		cameraSetBinding.descriptorCount = 1;
		cameraSetBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		cameraSetBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutCreateInfo setInfo{};
		setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		setInfo.bindingCount = 1;
		setInfo.pBindings = &cameraSetBinding;

		if (vkCreateDescriptorSetLayout(device.Get(),
			&setInfo, nullptr, &cameraLayout) != VK_SUCCESS)
			return false;
	}

	// Material set
	{
		VkDescriptorSetLayoutBinding samplerBinding{};
		samplerBinding.binding = 1;
		samplerBinding.descriptorCount = 1;
		samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo setInfo{};
		setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		setInfo.bindingCount = 1;
		setInfo.pBindings = &samplerBinding;

		if (vkCreateDescriptorSetLayout(device.Get(),
			&setInfo, nullptr, &materialLayout) != VK_SUCCESS)
			return false;
	}

	if (!cameraManager.Init(&device, 1, swapchainImages))
		return false;
	if (!cameraUniform.Init(this, sizeof(Objects::Camera::CameraUniforms), 
		swapchain.GetImageCount()))
		return false;

	cameraUniform.BindToSet(&cameraManager, cameraLayout);
	cameraUniform.Bind(0);

	return true;
}

bool VulkanRenderer::InitPipeline()
{
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

	VkDynamicState dynamicStates[] =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkDescriptorSetLayout sets[] =
	{
		objectLayout,
		cameraLayout,
		materialLayout
	};

	PipelineInfo pipelineInfo{};
	pipelineInfo.stageCount = sizeof(stages) / sizeof(stages[0]);
	pipelineInfo.pStages = stages;
	pipelineInfo.pDynamicStates = dynamicStates;
	pipelineInfo.dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
	pipelineInfo.pDynamicStates = dynamicStates;
	pipelineInfo.setCount = sizeof(sets) / sizeof(sets[0]);
	pipelineInfo.pSetLayouts = sets;
	
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
	Objects::Camera* pCamera = pApplication->GetActiveCamera();
	if (pCamera)
		if (pCamera->IsEnabled())
		{
			// Upload camera matrices

			cameraUniform.UpdateBuffer(pCamera->GetUniforms(), 
				sizeof(Objects::Camera::CameraUniforms), imageIndex);
		}

	std::vector<Objects::Object*>* defaultObjects = pApplication->GetDefaultScene()
		->GetObjects();
	for (uint64_t i = 0; i < defaultObjects->size(); i++)
	{
		ObjectNative* vkNative = (ObjectNative*)defaultObjects->at(i)->GetNative();

		vkNative->SetCurrentImage(imageIndex);
		vkNative->OnUniformsUpdate();
	}

	std::vector<Objects::Object*>* activeObjects = pApplication->GetActiveScene()
		->GetObjects();
	for (uint64_t i = 0; i < activeObjects->size(); i++)
	{
		ObjectNative* vkNative = (ObjectNative*)activeObjects->at(i)->GetNative();

		vkNative->SetCurrentImage(imageIndex);
		vkNative->OnUniformsUpdate();
	}
}

bool VulkanRenderer::DrawFrame()
{
	if (shouldRecreateSwapchain)
		if (!RecreateSwapchain())
			pApplication->Log("Failed to recreate swapchain!", LogType::ERROR);

	if (shouldRecreateCommandBuffers)
	{
		if (!RecreateCommandBuffers())
			pApplication->Log("Failed to recreate command buffers!", LogType::ERROR);

		shouldRecreateCommandBuffers = false;
	}

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

bool VulkanRenderer::CreateObjectNative(Objects::Object* pObject)
{
	LEGENDENGINE_ASSERT_INITIALIZED_RET(false);

	if (!pObject)
	{
		pApplication->Log(
			"Creating object native: Object is nullptr. Returning.",
			LogType::WARN);
		return false;
	}

	pObject->SetNative(RefTools::Create<ObjectNative>(this, pObject));
	return true;
}

bool VulkanRenderer::CreateVertexBufferNative(LegendEngine::VertexBuffer* buffer)
{
	LEGENDENGINE_ASSERT_INITIALIZED_RET(false);

	if (!buffer)
	{
		pApplication->Log(
			"Creating vertex buffer native: Buffer is nullptr. Returning.",
			LogType::WARN);
		return false;
	}

	buffer->SetNative(RefTools::Create<VertexBufferNative>(this, buffer));

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

		// This will probably be changed later on
		// Eventually, objects will have to be rendered in order of distance
		// from the camera.

		// Default scene
		Scene* pDefault = pApplication->GetDefaultScene();
		PopulateByScene(buffer, framebuffer, commandBufferIndex, pDefault);

		// ActiveScene
		Scene* pActive = pApplication->GetActiveScene();
		PopulateByScene(buffer, framebuffer, commandBufferIndex, pActive);
	}
	vkCmdEndRenderPass(buffer);
	
	if (vkEndCommandBuffer(buffer) != VK_SUCCESS)
		return false;

	return true;
}

void VulkanRenderer::PopulateByScene(VkCommandBuffer buffer, 
	VkFramebuffer framebuffer, uint64_t commandBufferIndex, Scene* pScene)
{
	if (!pScene)
		return;

	using namespace Objects;
	using namespace Objects::Components;

	static const std::string meshCompName = TypeTools::GetTypeName<MeshComponent>();

	auto* comps = pScene->GetObjectComponents();
	if (comps->find(meshCompName) == comps->end())
		return;

	std::vector<Component*>* meshComps = &comps->at(meshCompName);

	VkDescriptorSet sets[2] = {};
	cameraUniform.GetDescriptorSet(&sets[1], commandBufferIndex);

	for (uint64_t i = 0; i < meshComps->size(); i++)
	{
		MeshComponent* component = (MeshComponent*)meshComps->at(i);
		Object* object = component->GetObject();

		if (!object->IsEnabled())
			continue;

		LegendEngine::VertexBuffer* pVertexBuffer =
			component->GetVertexBuffer();
		VertexBufferNative* pVkVertexBuffer =
			(VertexBufferNative*)pVertexBuffer->GetNative();
		
		ObjectNative* native = (ObjectNative*)object->GetNative();
		native->GetUniform()->GetDescriptorSet(&sets[0], commandBufferIndex);

		vkCmdBindDescriptorSets(
			buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			shaderProgram.GetPipelineLayout(), 0, sizeof(sets) / sizeof(sets[0]), 
			sets, 0, nullptr
		);

		VkBuffer vbuffers[] = { pVkVertexBuffer->vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(buffer, 0, 1, vbuffers, offsets);

		vkCmdDraw(buffer, component->GetVertexCount(), 1, 0, 0);
	}
}

void VulkanRenderer::DisposeSwapchain()
{
	vkFreeCommandBuffers(device.Get(), commandPool, 
		static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

	for (VkFramebuffer framebuffer : framebuffers)
		vkDestroyFramebuffer(device.Get(), framebuffer, nullptr);
	
	for (VkImageView imageView : swapchainImageViews)
		vkDestroyImageView(device.Get(), imageView, nullptr);

	swapchain.Dispose();
}

#endif // VULKAN_API