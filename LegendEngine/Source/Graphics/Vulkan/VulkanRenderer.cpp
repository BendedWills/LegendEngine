#include <LegendEngine/Application.hpp>
#include <LegendEngine/Components/MeshComponent.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanMaterial.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanVertexBuffer.hpp>
#include <Tether/Rendering/Vulkan/SingleUseCommandBuffer.hpp>

#include <LegendEngine/Graphics/Vulkan/VulkanRenderTargetBridge.hpp>

// #ifdef VMA_VULKAN_VERSION
// #undef VMA_VULKAN_VERSION
// #endif
//
// #define VMA_VULKAN_VERSION 1000000
// #define VMA_IMPLEMENTATION
// #include <vk_mem_alloc.h>

namespace LegendEngine::Graphics::Vulkan
{
    using namespace Resources;

    VulkanRenderer::VulkanRenderer(
            TetherVulkan::GraphicsContext& tetherCtx,
            RenderTarget& renderTarget,
            ShaderManager& shaderManager,
            const VkDescriptorSetLayout cameraLayout,
            const VkDescriptorSetLayout sceneLayout,
            VkSurfaceFormatKHR surfaceFormat,
            TetherVulkan::DescriptorSet& defaultMatSet
            )
        :
        Renderer(renderTarget),
        m_Context(tetherCtx),
        m_DeviceLoader(tetherCtx.GetDeviceLoader()),
        m_DefaultMatSet(defaultMatSet),
        m_ShaderManager(shaderManager),
        m_Surface(static_cast<VulkanRenderTargetBridge&>(renderTarget.GetBridge()).GetSurface()),
        m_SurfaceFormat(surfaceFormat),
        m_Device(m_Context.GetDevice()),
        m_PhysicalDevice(m_Context.GetPhysicalDevice())
    {
        // Application::Get() doesn't work here

        const TetherVulkan::SwapchainDetails details = QuerySwapchainSupport();

        CreateSwapchain(details);
        CreateRenderPass();
        CreateUniforms(cameraLayout, sceneLayout);
        CreateDepthImages();
        CreateCommandBuffers();
        CreateSyncObjects();
    }

    VulkanRenderer::~VulkanRenderer()
    {
        LGENG_DEBUG_LOG("Destroying renderer");

        vkDeviceWaitIdle(m_Device);

        DestroySwapchain();

        vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);

        for (uint64_t i = 0; i < m_Context.GetFramesInFlight(); i++)
        {
            vkDestroySemaphore(m_Device, m_RenderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(m_Device, m_ImageAvailableSemaphores[i], nullptr);
            vkDestroyFence(m_Device, m_InFlightFences[i], nullptr);
        }

        LGENG_DEBUG_LOG("Destroyed renderer");
    }

    void VulkanRenderer::SetVSyncEnabled(const bool vsync)
    {
        m_VSync = vsync;
        RecreateSwapchain();
    }

    void VulkanRenderer::NotifyWindowResized()
    {
        m_ShouldRecreateSwapchain = true;
    }

    bool VulkanRenderer::StartFrame()
    {
        if (m_ShouldRecreateSwapchain)
            RecreateSwapchain();

        // in flight frame = a frame that is being rendered while still rendering
        // more frames

        // If this frame is still in flight, wait for it to finish rendering before
        // rendering another frame.
        vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame],
            VK_TRUE, UINT64_MAX);

        // The swapchain has one more than the minimum images, so the index
        // might not be the same as m_CurrentFrame
        const VkResult acquireResult = vkAcquireNextImageKHR(
            m_Device, m_Swapchain->Get(),
            UINT64_MAX,
            m_ImageAvailableSemaphores[m_CurrentFrame],
            VK_NULL_HANDLE,
            &m_CurrentImageIndex
        );
        if (acquireResult != VK_SUCCESS)
        {
            m_ShouldRecreateSwapchain = true;
            return false;
        }

        BeginCommandBuffer();

        return true;
    }

    void VulkanRenderer::BeginCommandBuffer()
    {
        const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];
        const VkExtent2D swapchainExtent = m_Swapchain->GetExtent();

        vkResetCommandBuffer(buffer, 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS)
            throw std::runtime_error("Failed to begin recording command buffer");

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchainExtent.width);
        viewport.height = static_cast<float>(swapchainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = swapchainExtent.width;
        scissor.extent.height = swapchainExtent.height;

        VkRenderingAttachmentInfoKHR colorAttachmentInfo{};
        colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        colorAttachmentInfo.imageView = m_SwapchainImageViews[m_CurrentFrame];
        colorAttachmentInfo.clearValue = { 0.0f, 0.0f, 0.0f, 1.0f };

        VkRenderingAttachmentInfoKHR depthAttachmentInfo{};
        depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        depthAttachmentInfo.imageView = m_DepthImageView;
        depthAttachmentInfo.clearValue = {1.0f, 0.0f };

        VkRenderingInfoKHR renderInfo{};
        renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderInfo.colorAttachmentCount = 1;
        renderInfo.pColorAttachments = &colorAttachmentInfo;
        renderInfo.layerCount = 1;
        renderInfo.pDepthAttachment = &depthAttachmentInfo;
        renderInfo.pStencilAttachment = &depthAttachmentInfo;
        renderInfo.renderArea.offset = { 0, 0 };
        renderInfo.renderArea.extent = swapchainExtent;

        m_DeviceLoader.vkCmdBeginRenderingKHR(buffer, &renderInfo);

        vkCmdSetViewport(buffer, 0, 1, &viewport);
        vkCmdSetScissor(buffer, 0, 1, &scissor);

        m_Sets[0] = *m_CameraSet->GetSetAtIndex(m_CurrentFrame);
        m_Sets[1] = *m_SceneSet->GetSetAtIndex(m_CurrentFrame);
        m_Sets[2] = *m_DefaultMatSet.GetSetAtIndex(m_CurrentFrame);
        m_pCurrentShader = static_cast<VulkanShader*>(m_ShaderManager.GetByID("solid"));

        vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pCurrentShader->GetPipeline());
    }

    void VulkanRenderer::UseMaterial(Material* pMaterial)
    {
        m_CurrentlyUsingMaterial = pMaterial;
        const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];

        auto pShader = static_cast<VulkanShader*>(m_ShaderManager.GetByID("solid"));

        if (pMaterial)
        {
            m_Sets[2] = static_cast<VulkanMaterial*>(pMaterial)->GetSetAtIndex(m_CurrentFrame);
            pShader = static_cast<VulkanShader*>(pMaterial->GetShader());
        }
        else
            m_Sets[2] = *m_DefaultMatSet.GetSetAtIndex(m_CurrentFrame);

        if (pShader == m_pCurrentShader)
            return;

        vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                pShader->GetPipeline());
        m_pCurrentShader = pShader;
    }

    void VulkanRenderer::DrawMesh(const Components::MeshComponent& mesh)
    {
        const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];
        const Objects::Object& object = mesh.GetObject();
        auto& vertexBuffer = static_cast<VulkanVertexBuffer&>(
            mesh.GetVertexBuffer());

        Pipeline::ObjectTransform transform;
        transform.transform = object.GetTransformationMatrix();

        vkCmdPushConstants(buffer, m_pCurrentShader->GetPipelineLayout(),
            VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(transform),
            &transform);

        // The sets that have a chance of not being used should be at the end
        // of m_Sets
        const size_t setCount = std::size(m_Sets) - 1 + m_CurrentlyUsingMaterial;

        vkCmdBindDescriptorSets(
            buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pCurrentShader->GetPipelineLayout(),
            0, setCount,
            m_Sets,
            0, nullptr
        );

        const VkBuffer vBuffers[] = { vertexBuffer.GetVertexBuffer() };
        constexpr VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(buffer, 0, 1, vBuffers, offsets);

        vkCmdBindIndexBuffer(
            buffer,
            vertexBuffer.GetIndexBuffer(),
            0,
            VK_INDEX_TYPE_UINT32
        );

        vkCmdDrawIndexed(buffer, mesh.GetIndexCount(), 1, 0, 0, 0);
    }

    void VulkanRenderer::EndFrame()
    {
        const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];

        m_DeviceLoader.vkCmdEndRenderingKHR(buffer);
        if (vkEndCommandBuffer(buffer) != VK_SUCCESS)
            throw std::runtime_error("Failed to record render command buffer");

        // Wait for the image to be available before rendering the frame and
        // signal the render finished semaphore once rendering is complete.
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        const VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
        const VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };
        constexpr VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        // The in flight fence for this frame must be reset.
        vkResetFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame]);
        if (vkQueueSubmit(m_Context.GetQueue(), 1, &submitInfo,
            m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS)
            throw std::runtime_error("vkQueueSubmit");

        // Wait for the frame to be rendered until presenting
        // (hence the wait semaphores being the signal semaphores)
        VkSwapchainKHR swapchains[] = { m_Swapchain->Get() };
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &m_CurrentImageIndex;

        vkQueuePresentKHR(m_Context.GetQueue(), &presentInfo);

        m_CurrentFrame = (m_CurrentFrame + 1) % m_Context.GetFramesInFlight();
    }

    void VulkanRenderer::UpdateCameraUniforms(const Objects::Camera& camera)
    {
        CameraUniforms uniforms;
        uniforms.projection = camera.GetProjectionMatrix();
        uniforms.view = camera.GetViewMatrix();

        for (uint32_t i = 0; i < m_Context.GetFramesInFlight(); i++)
        {
            void* data = m_CameraUniforms->GetMappedData(i);
            *static_cast<CameraUniforms*>(data) = uniforms;
        }
    }

    void VulkanRenderer::CreateSwapchain(const TetherVulkan::SwapchainDetails& details)
    {
        m_Swapchain.emplace(m_Context, details, m_SurfaceFormat,
            m_Surface, m_RenderTarget.GetWidth(), m_RenderTarget.GetHeight(),
            m_VSync);

        m_SwapchainImages = m_Swapchain->GetImages();
        m_SwapchainImageViews = m_Swapchain->CreateImageViews();
    }

    void VulkanRenderer::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment{};
		colorAttachment.format = m_SurfaceFormat.format;
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

		const VkAttachmentDescription attachments[] =
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

		if (vkCreateRenderPass(m_Device, &desc, nullptr, &m_RenderPass)
			!= VK_SUCCESS)
			throw std::runtime_error("Failed to create render pass");
    }

    void VulkanRenderer::CreateUniforms(VkDescriptorSetLayout cameraLayout,
        VkDescriptorSetLayout sceneLayout)
    {
        const uint32_t framesInFlight = m_Context.GetFramesInFlight();

        VkDescriptorPoolSize uniformsSize{};
        uniformsSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformsSize.descriptorCount = framesInFlight * 2;

        VkDescriptorPoolSize samplersSize{};
        samplersSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplersSize.descriptorCount = framesInFlight;

        VkDescriptorPoolSize sizes[] =
        {
            uniformsSize,
            samplersSize
        };

        // 3 for two uniforms and one sampler
        m_StaticUniformPool.emplace(m_Context, framesInFlight * 3,
            std::size(sizes), sizes);

        m_CameraSet.emplace(*m_StaticUniformPool, cameraLayout, framesInFlight);
        m_CameraUniforms.emplace(m_Context, sizeof(CameraUniforms), *m_CameraSet,
            0);

        m_SceneSet.emplace(*m_StaticUniformPool, sceneLayout, framesInFlight);
        m_SceneUniforms.emplace(m_Context, sizeof(SceneUniforms), *m_SceneSet, 0);
    }

    void VulkanRenderer::CreateDepthImages()
    {
        auto [width, height] = m_Swapchain->GetExtent();

        Tether::Rendering::Resources::BufferedImageInfo info{};
        info.width = width;
        info.height = height;

        VkFormat depthFormat = FindDepthFormat();
        m_DepthImage.emplace(m_Context, nullptr,
            nullptr, info, depthFormat, true);
        m_DepthImageView = m_DepthImage->GetImageView();

        TetherVulkan::SingleUseCommandBuffer cmdBuffer(m_Context);
        cmdBuffer.Begin();
        cmdBuffer.TransitionImageLayout(m_DepthImage->Get(), depthFormat,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            VK_IMAGE_ASPECT_DEPTH_BIT);
        cmdBuffer.End();
        cmdBuffer.Submit();
    }

    void VulkanRenderer::CreateCommandBuffers()
    {
        m_CommandBuffers.resize(m_Context.GetFramesInFlight());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_Context.GetCommandPool();
        allocInfo.commandBufferCount = m_CommandBuffers.size();

        if (vkAllocateCommandBuffers(m_Device, &allocInfo,
                m_CommandBuffers.data()) != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate command buffers");
    }

    void VulkanRenderer::CreateSyncObjects()
    {
        const uint32_t framesInFlight = m_Context.GetFramesInFlight();
        m_ImageAvailableSemaphores.resize(framesInFlight);
        m_RenderFinishedSemaphores.resize(framesInFlight);
        m_InFlightFences.resize(framesInFlight);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (uint32_t i = 0; i < framesInFlight; i++)
        {
            if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr,
                &m_ImageAvailableSemaphores[i]) != VK_SUCCESS)
                throw std::runtime_error("Failed to create semaphore");
            if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr,
                &m_RenderFinishedSemaphores[i]) != VK_SUCCESS)
                throw std::runtime_error("Failed to create semaphore");
            if (vkCreateFence(m_Device, &fenceInfo, nullptr,
                &m_InFlightFences[i]) != VK_SUCCESS)
                throw std::runtime_error("Failed to create fence");
        }
    }

    TetherVulkan::SwapchainDetails VulkanRenderer::QuerySwapchainSupport() const
    {
        TetherVulkan::SwapchainDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            m_PhysicalDevice,
            m_Surface,
            &details.capabilities
        );

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            m_PhysicalDevice,
            m_Surface,
            &formatCount,
            nullptr
        );

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(
                m_PhysicalDevice,
                m_Surface,
                &formatCount,
                details.formats.data()
            );
        }

        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            m_PhysicalDevice,
            m_Surface,
            &presentModeCount,
            nullptr
        );

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                m_PhysicalDevice,
                m_Surface,
                &presentModeCount,
                details.presentModes.data()
            );
        }

        return details;
    }

    VkFormat VulkanRenderer::FindDepthFormat() const
    {
        constexpr VkFormat candidates[] = {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
        };

        constexpr VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

        for (const VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);

            if ((props.optimalTilingFeatures & features) == features)
                return format;
        }

        return candidates[0];
    }

    VkFormat VulkanRenderer::FindSupportedFormat(const std::vector<VkFormat>& candidates,
        const VkImageTiling tiling, const VkFormatFeatureFlags features) const
    {
        for (const VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR
                && (props.linearTilingFeatures & features) == features)
                return format;
            if (tiling == VK_IMAGE_TILING_OPTIMAL
                && (props.optimalTilingFeatures & features) == features)
                return format;
        }

        return candidates[0];
    }

    void VulkanRenderer::RecreateSwapchain()
    {
        // The m_Device might still have work. Wait for it to finish before
        // recreating the m_Swapchain->
        vkDeviceWaitIdle(m_Device);

        const TetherVulkan::SwapchainDetails details = QuerySwapchainSupport();

        if (details.capabilities.currentExtent.width == 0 ||
            details.capabilities.currentExtent.height == 0)
        {
            m_ShouldRecreateSwapchain = false;
            return;
        }

        DestroySwapchain();

        CreateSwapchain(details);
        CreateDepthImages();
        CreateCommandBuffers();

        m_ShouldRecreateSwapchain = false;
    }

    void VulkanRenderer::DestroySwapchain()
    {
        vkFreeCommandBuffers(m_Device, m_Context.GetCommandPool(),
            static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());

        m_DepthImage.reset();

        for (const VkImageView imageView : m_SwapchainImageViews)
            vkDestroyImageView(m_Device, imageView, nullptr);

        m_Swapchain.reset();
    }
}
