#ifdef VULKAN_API

#include <LegendEngine/VulkanRenderer.hpp>

#include <set>

using namespace LegendEngine;

void VulkanRenderer::DebugCallback::OnDebugLog(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
)
{
    std::stringstream ss;
    ss << "Vulkan Validation Layer: " << pCallbackData->pMessage;
    
    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        pApplication->Log(ss.str(), LogType::ERROR);
    else
        pApplication->Log(ss.str(), LogType::DEBUG);
}

void VulkanRenderer::SetValidationLayersEnabled(bool enableValidationLayers)
{
    this->enableValidationLayers = enableValidationLayers;
}

bool VulkanRenderer::OnRendererInit()
{
    callback.pApplication = GetApplication();

    instance.AddDebugMessenger(&callback);
    if (!instance.Init(GetApplication()->GetName().c_str(), "LegendEngine", 
        enableValidationLayers))
        return false;

    if (!surface.Init(&instance, GetApplication()->GetWindow()))
        return false;
    
    return true;
}

void VulkanRenderer::OnRendererDispose()
{
    swapchain.Dispose();
    
    device.Dispose();
    surface.Dispose();
    instance.Dispose();
}

bool VulkanRenderer::PickDevice(VkPhysicalDevice* pDevice, 
    Vulkan::Surface* pSurface)
{
    uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance.Get(), &deviceCount, nullptr);
	if (deviceCount == 0)
		return false;
	
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance.Get(), &deviceCount, 
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
        instance.FindQueueFamilies(device, pSurface);

	bool extentionsSupported = instance.CheckDeviceExtentionSupport(device,
        deviceExtensions.data(), deviceExtensions.size());

	bool swapChainGood = false;
	if (extentionsSupported)
	{
		Vulkan::SwapchainDetails details = instance.QuerySwapchainSupport(device, 
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
            &instance, 
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
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB 
			&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return availableFormat;

    return details.formats[0];
}

bool VulkanRenderer::InitSwapchain(uint64_t width, uint64_t height)
{
    Vulkan::SwapchainDetails details = 
        instance.QuerySwapchainSupport(physicalDevice, &surface);
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
	createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	createInfo.clipped = true;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

    indices = instance.FindQueueFamilies(physicalDevice, &surface);
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

#endif // VULKAN_API