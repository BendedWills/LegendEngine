#include <LegendEngine/Application.hpp>
#include <LegendEngine/Components/MeshComponent.hpp>
#include <LegendEngine/Graphics/Vulkan/VkDefs.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanMaterial.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanVertexBuffer.hpp>
#include <Tether/Rendering/Vulkan/SingleUseCommandBuffer.hpp>

#include <LegendEngine/Graphics/Vulkan/VulkanRenderTarget.hpp>

// #ifdef VMA_VULKAN_VERSION
// #undef VMA_VULKAN_VERSION
// #endif
//
// #define VMA_VULKAN_VERSION 1000000
// #define VMA_IMPLEMENTATION
// #include <vk_mem_alloc.h>

namespace le
{
    VulkanRenderer::VulkanRenderer(
            TetherVulkan::GraphicsContext& tetherCtx,
            RenderTarget& renderTarget,
            ShaderManager& shaderManager,
            VkDescriptorSetLayout cameraLayout,
            VkDescriptorSetLayout sceneLayout,
            VkSurfaceFormatKHR surfaceFormat,
            TetherVulkan::DescriptorSet& defaultMatSet,
            VkFormat depthFormat,
            std::mutex& graphicsQueueMutex
            )
        :
        Renderer(renderTarget),
        m_Context(tetherCtx),
        m_DeviceLoader(tetherCtx.GetDeviceLoader()),
        m_DefaultMatSet(defaultMatSet),
        m_ShaderManager(shaderManager),
        m_Surface(dynamic_cast<VulkanRenderTarget&>(renderTarget).GetSurface()),
        m_SurfaceFormat(surfaceFormat),
        m_Device(m_Context.GetDevice()),
        m_PhysicalDevice(m_Context.GetPhysicalDevice()),
        m_DepthFormat(depthFormat),
        m_GraphicsQueueMutex(graphicsQueueMutex)
    {
        LE_ASSERT(!dynamic_cast<VulkanRenderTarget&>(renderTarget).IsHeadless(),
            "Renderers can't be created with a headless surface");

        // Application::Get() doesn't work here

        m_DepthImages.resize(m_Context.GetFramesInFlight());
        m_DepthAllocs.resize(m_Context.GetFramesInFlight());
        m_DepthImageViews.resize(m_Context.GetFramesInFlight());

        const TetherVulkan::SwapchainDetails details = QuerySwapchainSupport();

        CreateSwapchain(details);
        CreateUniforms(cameraLayout, sceneLayout);
        CreateDepthImages();
        CreateCommandBuffers();
        CreateSyncObjects();
    }

    VulkanRenderer::~VulkanRenderer()
    {
        LE_DEBUG("Destroying renderer");

        vkDeviceWaitIdle(m_Device);

        DestroySwapchain();

        for (uint64_t i = 0; i < m_Context.GetFramesInFlight(); i++)
        {
            vkDestroySemaphore(m_Device, m_ImageAvailableSemaphores[i], nullptr);
            vkDestroyFence(m_Device, m_InFlightFences[i], nullptr);
        }

        for (uint32_t i = 0; i < m_SwapchainImageCount; i++)
        {
            vkDestroySemaphore(m_Device, m_RenderFinishedSemaphores[i], nullptr);
        }

        LE_DEBUG("Destroyed renderer");
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
        vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame], true, UINT64_MAX);

        if (m_IsFrameWaiting && m_FrameWithStagerWaits == m_CurrentFrame)
        {
            // The fence for this frame has been waited on. This frame had the
            // semaphores for the stager completion, so it's safe to delete it
            // here.
            m_WaitingForStagerDeletion->DeleteStager();
            m_IsFrameWaiting = false;
            m_ShouldWaitForStager = false;
            m_WaitingForStagerDeletion = nullptr;
        }

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

        VkImageMemoryBarrier colorBarrier{};
        colorBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        colorBarrier.image = m_SwapchainImages[m_CurrentImageIndex];
        colorBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        colorBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        vkCmdPipelineBarrier(buffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0, 0, nullptr,
            0, nullptr,
            1, &colorBarrier);

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
        colorAttachmentInfo.imageView = m_SwapchainImageViews[m_CurrentImageIndex];
        colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachmentInfo.clearValue =
            {
                m_ClearColor.x,
                m_ClearColor.y,
                m_ClearColor.z,
                m_ClearColor.w
            };
        colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingAttachmentInfoKHR depthAttachmentInfo{};
        depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        depthAttachmentInfo.imageView = m_DepthImageViews[m_CurrentFrame];
        depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachmentInfo.clearValue = {1.0f, 0.0f };
        depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingInfoKHR renderInfo{};
        renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderInfo.colorAttachmentCount = 1;
        renderInfo.pColorAttachments = &colorAttachmentInfo;
        renderInfo.layerCount = 1;
        renderInfo.pDepthAttachment = &depthAttachmentInfo;
        renderInfo.pStencilAttachment = nullptr;
        renderInfo.renderArea.offset = { 0, 0 };
        renderInfo.renderArea.extent = swapchainExtent;

        m_DeviceLoader.vkCmdBeginRenderingKHR(buffer, &renderInfo);

        vkCmdSetViewport(buffer, 0, 1, &viewport);
        vkCmdSetScissor(buffer, 0, 1, &scissor);

        m_Sets[0] = *m_CameraSet->GetSetAtIndex(m_CurrentFrame);
        m_Sets[1] = *m_SceneSet->GetSetAtIndex(m_CurrentFrame);
        m_Sets[2] = *m_DefaultMatSet.GetSetAtIndex(m_CurrentFrame);
        m_pCurrentShader = static_cast<VulkanShader*>(m_ShaderManager.GetByID("solid"));
        m_HaveSetsChanged = true;

        vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pCurrentShader->GetPipeline());
    }

    void VulkanRenderer::UseMaterial(Material* pMaterial)
    {
        const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];

        auto pShader = static_cast<VulkanShader*>(m_ShaderManager.GetByID("solid"));

        if (pMaterial)
        {
            m_Sets[2] = static_cast<VulkanMaterial*>(pMaterial)->GetSetAtIndex(m_CurrentFrame);
            pShader = static_cast<VulkanShader*>(pMaterial->GetShader());
        }
        else
            m_Sets[2] = *m_DefaultMatSet.GetSetAtIndex(m_CurrentFrame);

        m_HaveSetsChanged = true;

        if (pShader == m_pCurrentShader)
            return;

        vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                pShader->GetPipeline());
        vkCmdSetCullMode(buffer, pShader->GetVulkanCullMode());

        m_pCurrentShader = pShader;
    }

    void VulkanRenderer::DrawMesh(const MeshComponent& mesh)
    {
        const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];
        const Object& object = mesh.GetObject();
        Ref<VulkanVertexBuffer> vertexBuffer =
            std::static_pointer_cast<VulkanVertexBuffer>(mesh.GetVertexBuffer());

        vertexBuffer->DeleteUnusedBuffers(m_CurrentFrame);

        const VulkanVertexBuffer::BufferInfo info = vertexBuffer->GetBufferInfo();
        if (!info.vertex)
            return;

        vertexBuffer->Use(m_CurrentFrame);

        if (!m_ShouldWaitForStager && vertexBuffer->ShouldWait())
        {
            m_WaitingForStagerDeletion = vertexBuffer;
            m_StagerSemaphoreValue = vertexBuffer->StartWait();
            m_ShouldWaitForStager = true;
        }

        Pipeline::ObjectTransform transform;
        transform.transform = object.GetTransformationMatrix();

        vkCmdPushConstants(buffer, m_pCurrentShader->GetPipelineLayout(),
            VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(transform),
            &transform);

        if (m_HaveSetsChanged)
        {
            vkCmdBindDescriptorSets(
                buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                m_pCurrentShader->GetPipelineLayout(),
                0, std::size(m_Sets),
                m_Sets,
                0, nullptr
            );
            m_HaveSetsChanged = false;
        }

        const VkBuffer vBuffers[] = { info.vertex };
        constexpr VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(buffer, 0, 1, vBuffers, offsets);

        vkCmdBindIndexBuffer(
            buffer,
            info.index,
            0,
            VK_INDEX_TYPE_UINT32
        );

        vkCmdDrawIndexed(buffer, info.indexCount, 1, 0, 0, 0);
    }

    void VulkanRenderer::EndCommandBuffer() const
    {
        const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];

        vkCmdEndRendering(buffer);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = m_SwapchainImages[m_CurrentImageIndex];
        barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        vkCmdPipelineBarrier(buffer,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0, 0,
            nullptr, 0,
            nullptr, 1, &barrier);
    }

    void VulkanRenderer::EndFrame()
    {
        const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];

        EndCommandBuffer();

        const VkResult result = vkEndCommandBuffer(buffer);
        LE_ASSERT(result == VK_SUCCESS, "Failed to end command buffer");

        size_t waitSemaphoreCount = 1;

        VkSemaphoreSubmitInfo submitInfos[2]{};
        submitInfos[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        submitInfos[0].stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        submitInfos[0].semaphore = m_ImageAvailableSemaphores[m_CurrentFrame];

        if (!m_IsFrameWaiting && m_ShouldWaitForStager)
        {
            waitSemaphoreCount++;
            m_IsFrameWaiting = true;
            m_FrameWithStagerWaits = m_CurrentFrame;

            submitInfos[1].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
            submitInfos[1].stageMask = VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT;
            submitInfos[1].semaphore = m_WaitingForStagerDeletion->GetSemaphore();
            submitInfos[1].value = m_StagerSemaphoreValue;
        }

        VkSemaphoreSubmitInfo signalInfo{};
        signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        signalInfo.semaphore = m_RenderFinishedSemaphores[m_CurrentImageIndex];

        VkCommandBufferSubmitInfo commandBufferInfo{};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        commandBufferInfo.commandBuffer = m_CommandBuffers[m_CurrentFrame];

        // Wait for the image to be available before rendering the frame and
        // signal the render finished semaphore once rendering is complete.
        VkSubmitInfo2 submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        submitInfo.waitSemaphoreInfoCount = waitSemaphoreCount;
        submitInfo.pWaitSemaphoreInfos = submitInfos;
        submitInfo.commandBufferInfoCount = 1;
        submitInfo.pCommandBufferInfos = &commandBufferInfo;
        submitInfo.signalSemaphoreInfoCount = 1;
        submitInfo.pSignalSemaphoreInfos = &signalInfo;

        // The in flight fence for this frame must be reset.
        vkResetFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame]);
        m_GraphicsQueueMutex.lock();
        LE_CHECK_VK(vkQueueSubmit2(m_Context.GetQueue(), 1, &submitInfo,
            m_InFlightFences[m_CurrentFrame]));
        m_GraphicsQueueMutex.unlock();

        // Wait for the frame to be rendered until presenting
        // (hence the wait semaphores being the signal semaphores)
        VkSwapchainKHR swapchains[] = { m_Swapchain->Get() };
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[m_CurrentImageIndex];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &m_CurrentImageIndex;

        m_GraphicsQueueMutex.lock();
        vkQueuePresentKHR(m_Context.GetQueue(), &presentInfo);
        m_GraphicsQueueMutex.unlock();

        m_CurrentFrame = (m_CurrentFrame + 1) % m_Context.GetFramesInFlight();
    }

    void VulkanRenderer::UpdateCameraUniforms(const Camera& camera)
    {
        Camera::CameraUniforms uniforms;
        uniforms.projection = camera.GetProjectionMatrix();
        uniforms.view = camera.GetViewMatrix();

        for (uint32_t i = 0; i < m_Context.GetFramesInFlight(); i++)
        {
            void* data = m_CameraUniforms->GetMappedData(i);
            *static_cast<Camera::CameraUniforms*>(data) = uniforms;
        }
    }

    void VulkanRenderer::CreateSwapchain(const TetherVulkan::SwapchainDetails& details)
    {
        m_Swapchain.emplace(m_Context, details, m_SurfaceFormat,
            m_Surface, m_RenderTarget.GetWidth(), m_RenderTarget.GetHeight(),
            m_VSync);

        m_SwapchainImages = m_Swapchain->GetImages();
        m_SwapchainImageViews = m_Swapchain->CreateImageViews();
        m_SwapchainImageCount = m_Swapchain->GetImageCount();
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
        m_CameraUniforms.emplace(m_Context, sizeof(Camera::CameraUniforms), *m_CameraSet,
            0);

        m_SceneSet.emplace(*m_StaticUniformPool, sceneLayout, framesInFlight);
        m_SceneUniforms.emplace(m_Context, sizeof(SceneUniforms), *m_SceneSet, 0);
    }

    void VulkanRenderer::CreateDepthImages()
    {
        auto [width, height] = m_Swapchain->GetExtent();

        TetherVulkan::SingleUseCommandBuffer cmdBuffer(m_Context);
        cmdBuffer.Begin();

        for (uint32_t i = 0; i < m_Context.GetFramesInFlight(); i++)
        {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = width;
            imageInfo.extent.height = height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = m_DepthFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

            VmaAllocationCreateInfo allocInfo{};
            allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

            if (vmaCreateImage(m_Context.GetAllocator(), &imageInfo, &allocInfo,
                &m_DepthImages[i], &m_DepthAllocs[i], nullptr) != VK_SUCCESS)
                throw std::runtime_error("Failed to create image");

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = m_DepthImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = m_DepthFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_Device, &viewInfo, nullptr,
                &m_DepthImageViews[i]) != VK_SUCCESS)
                throw std::runtime_error("Failed to create texture image view");

            cmdBuffer.TransitionImageLayout(m_DepthImages[i], m_DepthFormat,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                VK_IMAGE_ASPECT_DEPTH_BIT);
        }

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
        m_RenderFinishedSemaphores.resize(m_SwapchainImageCount);
        m_ImageAvailableSemaphores.resize(framesInFlight);
        m_InFlightFences.resize(framesInFlight);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (uint32_t i = 0; i < framesInFlight; i++)
        {
            if (vkCreateFence(m_Device, &fenceInfo, nullptr,
                &m_InFlightFences[i]) != VK_SUCCESS)
                throw std::runtime_error("Failed to create fence");
            if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr,
                &m_ImageAvailableSemaphores[i]) != VK_SUCCESS)
                throw std::runtime_error("Failed to create semaphore");
        }

        for (uint32_t i = 0; i < m_SwapchainImages.size(); i++)
        {
            if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr,
                &m_RenderFinishedSemaphores[i]) != VK_SUCCESS)
                throw std::runtime_error("Failed to create semaphore");
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

        for (const VkImageView imageView : m_SwapchainImageViews)
            vkDestroyImageView(m_Device, imageView, nullptr);

        for (uint32_t i = 0; i < m_Context.GetFramesInFlight(); i++)
        {
            vmaDestroyImage(m_Context.GetAllocator(), m_DepthImages[i], m_DepthAllocs[i]);
            vkDestroyImageView(m_Device, m_DepthImageViews[i], nullptr);
        }

        m_Swapchain.reset();
    }
}
