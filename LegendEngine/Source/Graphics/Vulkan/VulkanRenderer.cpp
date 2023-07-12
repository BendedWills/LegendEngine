#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Application.hpp>
#include <LegendEngine/GraphicsContext.hpp>

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

namespace LegendEngine::Vulkan
{
	VulkanRenderer::VulkanRenderer()
		:
		m_Instance(((Vulkan::GraphicsContext&)GraphicsContext::Get()).GetInstance()),
		m_Device(m_Instance)
	{

	}

	void VulkanRenderer::SetVSyncEnabled(bool vsync)
	{
		LEGENDENGINE_ASSERT_INITIALIZED();

		this->enableVsync = vsync;

		Reload();
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

	bool VulkanRenderer::CreateMaterialNative(Resources::Material* pMaterial)
	{
		LEGENDENGINE_ASSERT_INITIALIZED_RET(false);

		if (!pMaterial)
		{
			pApplication->Log(
				"Creating material native: Material is nullptr. Returning.",
				LogType::WARN);
			return false;
		}

		pMaterial->SetNative(RefTools::Create<MaterialNative>(this, pMaterial));

		return true;
	}

	bool VulkanRenderer::Reload()
	{
		LEGENDENGINE_ASSERT_INITIALIZED_RET(false);
		return RecreateSwapchain();
	}

	bool VulkanRenderer::BeginSingleUseCommandBuffer(VkCommandBuffer* pCommandBuffer)
	{
		VkCommandBufferAllocateInfo cmdAllocInfo{};
		cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdAllocInfo.commandPool = commandPool;
		cmdAllocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(m_Device.Get(), &cmdAllocInfo,
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
			vkFreeCommandBuffers(m_Device.Get(), commandPool, 1, &commandBuffer);
			return false;
		}

		// Wait for the data to finish uploading
		vkQueueWaitIdle(graphicsQueue);

		// Free the command buffer
		vkFreeCommandBuffers(m_Device.Get(), commandPool, 1, &commandBuffer);

		return true;
	}

	bool VulkanRenderer::CreateImageView(VkImageView* pImageView, VkImage image,
		VkFormat format, VkImageViewType viewType, VkImageAspectFlags aspflags)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = image;
		createInfo.format = format;
		createInfo.viewType = viewType;
		createInfo.subresourceRange.aspectMask = aspflags;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		return vkCreateImageView(m_Device.Get(), &createInfo, nullptr, pImageView)
			== VK_SUCCESS;
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
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
			&& newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			memoryBarrier.srcAccessMask = 0;
			memoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
				| VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else
			return false;

		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT)
				memoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		else
			memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

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

	VkFormat VulkanRenderer::FindSupportedFormat(const std::vector<VkFormat>& candidates,
		VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_Device.GetPhysicalDevice(), format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR
				&& (props.linearTilingFeatures & features) == features)
				return format;
			else if (tiling == VK_IMAGE_TILING_OPTIMAL
				&& (props.optimalTilingFeatures & features) == features)
				return format;
		}

		return candidates[0];
	}

	VkFormat VulkanRenderer::FindDepthFormat()
	{
		return FindSupportedFormat(
			{
				VK_FORMAT_D32_SFLOAT,
				VK_FORMAT_D32_SFLOAT_S8_UINT,
				VK_FORMAT_D24_UNORM_S8_UINT
			},
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	bool VulkanRenderer::RecreateSwapchain()
	{
		LEGENDENGINE_OBJECT_LOG(pApplication, "VulkanRenderer",
			"Recreating swapchain (GraphicalWindow resize)",
			LogType::DEBUG);

		// The m_Device might still have work. Wait for it to finish before 
		// recreating the swapchain.
		m_Device.WaitIdle();

		Tether::Window* pWindow = pApplication->GetWindow();

		DisposeSwapchain();
		if (!InitSwapchain(pWindow->GetWidth(), pWindow->GetHeight())
			|| !InitDepthImages()
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

		// Wait for all frames to finish rendering.
		// Command buffers cannot be reset during frame rendering.
		for (uint64_t i2 = 0; i2 < inFlightFences.size(); i2++)
			vkWaitForFences(m_Device.Get(), 1, &inFlightFences[i2], true, UINT64_MAX);

		for (uint64_t i = 0; i < commandBuffers.size(); i++)
		{
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

		VkClearValue clearColors[] =
		{
			{ 0.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, 0 }
		};

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchainExtent;
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearColors;

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

			cameraUniform.GetDescriptorSet(&sets[0], commandBufferIndex);

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

		Resources::Material* lastMaterial = nullptr;
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
			native->GetUniform()->GetDescriptorSet(&sets[2], commandBufferIndex);

			Resources::Material* pMaterial = component->GetMaterial();
			if (pMaterial)
			{
				if (pMaterial != lastMaterial)
				{
					MaterialNative* matNative = (MaterialNative*)pMaterial->GetNative();
					matNative->uniform.GetDescriptorSet(&sets[1], commandBufferIndex);

					lastMaterial = pMaterial;
				}
			}
			else
			{
				MaterialNative* matNative = (MaterialNative*)defaultMaterial->GetNative();
				matNative->uniform.GetDescriptorSet(&sets[1], commandBufferIndex);
			}

			vkCmdBindDescriptorSets(
				buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				shaderProgram.GetPipelineLayout(),
				0, sizeof(sets) / sizeof(sets[0]),
				sets,
				0, nullptr
			);

			VkBuffer vbuffers[] = { pVkVertexBuffer->vertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(buffer, 0, 1, vbuffers, offsets);

			vkCmdBindIndexBuffer(
				buffer,
				pVkVertexBuffer->indexBuffer,
				0,
				VK_INDEX_TYPE_UINT32
			);

			vkCmdDrawIndexed(buffer, component->GetVertexCount(), 1, 0, 0, 0);
		}
	}

	bool VulkanRenderer::OnRendererInit()
	{
		timer.Set();

		pApplication = GetApplication();
		pInstance = GraphicsContextVk::Get().GetVulkanInstance();

		if (!GraphicsContextVk::IsVulkanInitialized())
		{
			pApplication->Log("Vulkan was not initialized for the Context",
				LogType::ERROR);
			return false;
		}

		if (!surface.Init(pInstance, pApplication->pWindow))
			return false;

		if (!PickDevice(m_Device.GetPhysicalDevice(), &surface))
			return false;

		indices = pInstance->FindQueueFamilies(m_Device.GetPhysicalDevice(), &surface);

		if (!InitDevice())
		{
			pApplication->Log("Failed to initialize m_Device!", LogType::ERROR);
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

		if (!InitCommandPool())
		{
			pApplication->Log("Failed to initialize command pool!", LogType::ERROR);
			return false;
		}

		if (!InitDepthImages())
		{
			pApplication->Log("Failed to initialize depth image!", LogType::ERROR);
			return false;
		}

		if (!InitFramebuffers())
		{
			pApplication->Log("Failed to initialize framebuffers!", LogType::ERROR);
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

	void VulkanRenderer::OnObjectChange(Objects::Object* pObject)
	{
		shouldRecreateCommandBuffers = true;
	}

	void VulkanRenderer::OnSceneChange(Scene* pScene, Objects::Object* pObject)
	{
		shouldRecreateCommandBuffers = true;
	}

	void VulkanRenderer::OnResourceChange(Resources::IResource* pResource)
	{
		shouldRecreateCommandBuffers = true;
	}

	bool VulkanRenderer::OnRenderFrame()
	{
		return DrawFrame();
	}

	void VulkanRenderer::OnRendererDispose()
	{
		m_Device.WaitIdle();

		DisposeSwapchain();

		shaderProgram.Dispose();

		vkDestroyDescriptorSetLayout(m_Device.Get(), objectLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_Device.Get(), cameraLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_Device.Get(), materialLayout, nullptr);
		cameraUniform.Dispose();
		cameraManager.Dispose();

		vkDestroyRenderPass(m_Device.Get(), renderPass, nullptr);

		for (uint64_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(m_Device.Get(), renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(m_Device.Get(), imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(m_Device.Get(), inFlightFences[i], nullptr);
		}

		vkDestroyCommandPool(m_Device.Get(), commandPool, nullptr);

		vmaDestroyAllocator(allocator);
	}

	void VulkanRenderer::OnWindowResize()
	{
		shouldRecreateSwapchain = true;
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

		if (!m_Device.Init(
			pInstance,
			m_Device.GetPhysicalDevice(),
			queueCreateInfos.data(),
			static_cast<uint32_t>(queueCreateInfos.size()),
			features,
			deviceExtensions.data(),
			static_cast<uint32_t>(deviceExtensions.size())
		))
			return false;

		graphicsQueue = m_Device.GetDeviceQueue(indices.graphicsFamilyIndex, 0);
		presentQueue = m_Device.GetDeviceQueue(indices.presentFamilyIndex, 0);

		return true;
	}

	bool VulkanRenderer::InitAllocator()
	{
		VmaVulkanFunctions funcs{};
		funcs.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
		funcs.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo createInfo{};
		createInfo.vulkanApiVersion = VK_API_VERSION_1_0;
		createInfo.m_Device.GetPhysicalDevice() = m_Device.GetPhysicalDevice();
		createInfo.m_Device = m_Device.Get();
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
			pInstance->QuerySwapchainSupport(m_Device.GetPhysicalDevice(), &surface);
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

		indices = pInstance->FindQueueFamilies(m_Device.GetPhysicalDevice(), &surface);
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

		if (!swapchain.Init(&surface, &m_Device, details,
			&createInfo))
			return false;

		swapchainImages = swapchain.GetImages();
		if (!swapchain.GetImageViews(&swapchainImageViews))
			return false;

		return true;
	}

	bool VulkanRenderer::InitShaders()
	{
		vertexModule.Init(&m_Device, ShaderType::VERTEX);
		fragmentModule.Init(&m_Device, ShaderType::FRAG);

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

		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = FindDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentReference{};
		colorAttachmentReference.attachment = 0;
		colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentReference{};
		depthAttachmentReference.attachment = 1;
		depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentReference;
		subpass.pDepthStencilAttachment = &depthAttachmentReference;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
			| VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		VkAttachmentDescription attachments[] =
		{
			colorAttachment,
			depthAttachment
		};

		VkRenderPassCreateInfo desc{};
		desc.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		desc.attachmentCount = 2;
		desc.pAttachments = attachments;
		desc.subpassCount = 1;
		desc.pSubpasses = &subpass;
		desc.dependencyCount = 1;
		desc.pDependencies = &dependency;

		if (vkCreateRenderPass(m_Device.Get(), &desc, nullptr, &renderPass)
			!= VK_SUCCESS)
			return false;

		return true;
	}

	bool VulkanRenderer::InitUniforms()
	{
		uint32_t swapchainImages = swapchain.GetImageCount();

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

			if (vkCreateDescriptorSetLayout(m_Device.Get(),
				&setInfo, nullptr, &cameraLayout) != VK_SUCCESS)
				return false;
		}

		// Material set
		{
			VkDescriptorSetLayoutBinding uniformBinding{};
			uniformBinding.binding = 0;
			uniformBinding.descriptorCount = 1;
			uniformBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uniformBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			VkDescriptorSetLayoutBinding samplerBinding{};
			samplerBinding.binding = 1;
			samplerBinding.descriptorCount = 1;
			samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			VkDescriptorSetLayoutBinding bindings[] =
			{
				uniformBinding,
				samplerBinding
			};

			VkDescriptorSetLayoutCreateInfo setInfo{};
			setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			setInfo.bindingCount = sizeof(bindings) / sizeof(bindings[0]);
			setInfo.pBindings = bindings;

			if (vkCreateDescriptorSetLayout(m_Device.Get(),
				&setInfo, nullptr, &materialLayout) != VK_SUCCESS)
				return false;
		}

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

			if (vkCreateDescriptorSetLayout(m_Device.Get(),
				&setInfo, nullptr, &objectLayout) != VK_SUCCESS)
				return false;
		}

		if (!cameraManager.Init(&m_Device, 1, swapchainImages))
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
			cameraLayout,
			materialLayout,
			objectLayout
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

	bool VulkanRenderer::InitDepthImages()
	{
		uint64_t swapImageCount = swapchainImageViews.size();
		VkExtent2D swapchainExtent = swapchain.GetExtent();

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = swapchainExtent.width;
		imageInfo.extent.height = swapchainExtent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.format = FindDepthFormat();

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		if (vmaCreateImage(allocator, &imageInfo, &allocInfo,
			&depthImage, &depthAlloc, nullptr) != VK_SUCCESS)
			return false;

		if (!CreateImageView(&depthImageView, depthImage, imageInfo.format,
			VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_DEPTH_BIT))
			return false;

		ChangeImageLayout(depthImage, imageInfo.format, VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		return true;
	}

	bool VulkanRenderer::InitFramebuffers()
	{
		VkExtent2D swapchainExtent = swapchain.GetExtent();
		uint64_t imageViewCount = swapchainImageViews.size();

		framebuffers.resize(imageViewCount);

		for (uint64_t i = 0; i < imageViewCount; i++)
		{
			VkImageView attachments[] =
			{
				swapchainImageViews[i],
				depthImageView
			};

			VkFramebufferCreateInfo framebufferDesc{};
			framebufferDesc.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferDesc.renderPass = renderPass;
			framebufferDesc.attachmentCount = 2;
			framebufferDesc.pAttachments = attachments;
			framebufferDesc.width = swapchainExtent.width;
			framebufferDesc.height = swapchainExtent.height;
			framebufferDesc.layers = 1;

			if (vkCreateFramebuffer(m_Device.Get(), &framebufferDesc, nullptr,
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

		return vkCreateCommandPool(m_Device.Get(), &poolInfo, nullptr, &commandPool)
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

		if (vkAllocateCommandBuffers(m_Device.Get(), &allocInfo,
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
			if (vkCreateSemaphore(m_Device.Get(), &semaphoreInfo, nullptr,
				&imageAvailableSemaphores[i]) != VK_SUCCESS)
				return false;
			if (vkCreateSemaphore(m_Device.Get(), &semaphoreInfo, nullptr,
				&renderFinishedSemaphores[i]) != VK_SUCCESS)
				return false;
			if (vkCreateFence(m_Device.Get(), &fenceInfo, nullptr,
				&inFlightFences[i]) != VK_SUCCESS)
				return false;
		}

		return true;
	}

	void VulkanRenderer::UpdateUniforms(uint64_t imageIndex)
	{
		defaultMaterial->Update();

		Objects::Camera* pCamera = pApplication->GetActiveCamera();
		if (pCamera)
			if (pCamera->IsEnabled())
			{
				// Upload camera matrices

				cameraUniform.UpdateBuffer(pCamera->GetUniforms(),
					sizeof(Objects::Camera::CameraUniforms), imageIndex);
			}

		UpdateSceneUniforms(imageIndex, pApplication->GetDefaultScene());
		UpdateSceneUniforms(imageIndex, pApplication->GetActiveScene());
	}

	void VulkanRenderer::UpdateSceneUniforms(uint64_t imageIndex, Scene* pScene)
	{
		using namespace Objects;
		using namespace Objects::Components;
		using namespace LegendEngine::Resources;

		// Cache the name of the mesh component for finding it later
		static const std::string meshCompName = TypeTools::GetTypeName<MeshComponent>();

		// Get the list of mesh components for every object in the scene
		auto* comps = pScene->GetObjectComponents();
		if (comps->find(meshCompName) == comps->end())
			return;
		std::vector<Component*>* meshComps = &comps->at(meshCompName);

		// For every mesh component, find its object (if it is enabled) and update the
		// uniforms for that object (position, rotation, scale, etc), then get its material
		// and update it (if it has one).
		for (uint64_t i = 0; i < meshComps->size(); i++)
		{
			MeshComponent* component = (MeshComponent*)meshComps->at(i);
			Object* object = component->GetObject();

			if (!object->IsEnabled())
				continue;

			ObjectNative* vkNative = (ObjectNative*)object->GetNative();
			vkNative->SetCurrentImage(imageIndex);
			vkNative->OnUniformsUpdate();

			Material* material = component->GetMaterial();
			if (!material)
				continue;

			material->Update();
		}
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
		vkWaitForFences(m_Device.Get(), 1, &inFlightFences[currentFrame], VK_TRUE,
			UINT64_MAX);

		// Get the next swapchain image. The swapchain has the minimum
		// amount of images plus one.
		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(m_Device.Get(),
			swapchain.Get(), UINT64_MAX, imageAvailableSemaphores[currentFrame],
			VK_NULL_HANDLE, &imageIndex);
		// vkAcquireNextImageKHR might throw an error.
		// If it does throw an error, simply return true if it is suboptimal or
		// out of date.
		if (result != VK_SUCCESS)
		{
			shouldRecreateSwapchain = true;
			return result == VK_SUBOPTIMAL_KHR
				|| result == VK_ERROR_OUT_OF_DATE_KHR;
		}

		UpdateUniforms(imageIndex);

		if (shouldRecreateCommandBuffers)
		{
			if (!RecreateCommandBuffers())
				pApplication->Log("Failed to recreate command buffers!", LogType::ERROR);

			shouldRecreateCommandBuffers = false;
		}

		// If images are acquired out of order, or MAX_FRAMES_IN_FLIGHT is higher
		// than the number of swapchain images, we may start rendering to an
		// image that is in flight.
		// Check for that here.
		if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
			vkWaitForFences(m_Device.Get(), 1, &imagesInFlight[imageIndex], VK_TRUE,
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

		// The in flight fence for this frame must be reset.
		vkResetFences(m_Device.Get(), 1, &inFlightFences[currentFrame]);
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

	void VulkanRenderer::DisposeSwapchain()
	{
		vkFreeCommandBuffers(m_Device.Get(), commandPool,
			static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

		for (VkFramebuffer framebuffer : framebuffers)
			vkDestroyFramebuffer(m_Device.Get(), framebuffer, nullptr);

		// Destroy depth stuff
		vkDestroyImageView(m_Device.Get(), depthImageView, nullptr);
		vmaDestroyImage(allocator, depthImage, depthAlloc);

		for (VkImageView imageView : swapchainImageViews)
			vkDestroyImageView(m_Device.Get(), imageView, nullptr);

		swapchain.Dispose();
	}
}
