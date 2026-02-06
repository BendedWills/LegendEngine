#include <LegendEngine/Application.hpp>
#include <LegendEngine/Components/MeshComponent.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanMaterial.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanVertexBuffer.hpp>
#include <Tether/Rendering/Vulkan/SingleUseCommandBuffer.hpp>

#include <Assets/CompiledShaders/solid.vert.spv.h>
#include <Assets/CompiledShaders/solid.frag.spv.h>
#include <Assets/CompiledShaders/textured.vert.spv.h>
#include <Assets/CompiledShaders/textured.frag.spv.h>

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
    using namespace VulkanShaders;

    VulkanRenderer::VulkanRenderer(Application& app,
        TetherVulkan::GraphicsContext& tetherCtx)
        :
        Renderer(app),
        m_Context(tetherCtx),
        m_Window(app.GetWindow()),
        m_Surface(m_Context, m_Window),
        m_Device(m_Context.GetDevice()),
        m_PhysicalDevice(m_Context.GetPhysicalDevice())
    {
        // Application::Get() doesn't work here

#ifdef LEGENDENGINE_DEBUG
        app.GetLogger().Log(Logger::Level::DEBUG, "Creating renderer");
#endif

        const TetherVulkan::SwapchainDetails details = QuerySwapchainSupport();
        ChooseSurfaceFormat(details);

        CreateSwapchain(details);
        CreateRenderPass();
        CreateUniforms();
        CreateShaders();
        CreateDepthImages();
        CreateFramebuffers();
        CreateCommandBuffers();
        CreateSyncObjects();

#ifdef LEGENDENGINE_DEBUG
        app.GetLogger().Log(Logger::Level::DEBUG, "Created renderer");
#endif
    }

    VulkanRenderer::~VulkanRenderer()
    {
        LGENG_DEBUG_LOG("Destroying renderer");

        vkDeviceWaitIdle(m_Device);

        m_CameraUniforms.reset();
        m_DefaultMatUniforms.reset();
        m_DefaultMatSet.reset();
        m_CameraSet.reset();
        m_StaticUniformPool.reset();

        DestroySwapchain();

        m_SolidShader.reset();
        m_TexturedShader.reset();

        vkDestroyDescriptorSetLayout(m_Device, m_CameraLayout, nullptr);
        vkDestroyDescriptorSetLayout(m_Device, m_MaterialLayout, nullptr);
        vkDestroyDescriptorSetLayout(m_Device, m_SceneLayout, nullptr);

        vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);

        for (uint64_t i = 0; i < m_Context.GetFramesInFlight(); i++)
        {
            vkDestroySemaphore(m_Device, m_RenderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(m_Device, m_ImageAvailableSemaphores[i], nullptr);
            vkDestroyFence(m_Device, m_InFlightFences[i], nullptr);
        }

        LGENG_DEBUG_LOG("Destroyed renderer");
    }

    Scope<Shader> VulkanRenderer::CreateShader(std::span<Shader::Stage> stages)
    {
        VkDescriptorSetLayout sets[] =
        {
            m_CameraLayout,
            m_MaterialLayout,
        };

        return std::make_unique<VulkanShader>(m_Context, stages, sets, m_RenderPass);
    }

    Scope<Material> VulkanRenderer::CreateMaterial()
    {
        return std::make_unique<VulkanMaterial>(m_Context, m_MaterialLayout);
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
        const VkFramebuffer framebuffer = m_Framebuffers[m_CurrentImageIndex];
        const VkExtent2D swapchainExtent = m_Swapchain->GetExtent();

        vkResetCommandBuffer(buffer, 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS)
            throw std::runtime_error("Failed to begin recording command buffer");

        VkClearValue clearColors[] =
        {
            { 0.0f, 0.0f, 0.0f, 1.0f },
            { 1.0f, 0 }
        };

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_RenderPass;
        renderPassInfo.framebuffer = framebuffer;
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapchainExtent;
        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearColors;

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

        vkCmdBeginRenderPass(buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdSetViewport(buffer, 0, 1, &viewport);
        vkCmdSetScissor(buffer, 0, 1, &scissor);

        m_Sets[0] = *m_CameraSet->GetSetAtIndex(m_CurrentFrame);
        m_Sets[1] = *m_DefaultMatSet->GetSetAtIndex(m_CurrentFrame);
        m_pCurrentShader = &m_SolidShader.value();

        vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pCurrentShader->GetPipeline());
    }

    void VulkanRenderer::UseMaterial(Material* pMaterial)
    {
        const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];
        const auto pVkMat = static_cast<VulkanMaterial*>(pMaterial);

        VulkanShader* pShader = &m_SolidShader.value();
        if (const Texture2D* pTexture = pMaterial->GetTexture();
            pMaterial && pTexture)
        {
            m_Sets[1] = pVkMat->GetSetAtIndex(m_CurrentFrame);
            pShader = &m_TexturedShader.value();
        }
        else
            m_Sets[1] = *m_DefaultMatSet->GetSetAtIndex(m_CurrentFrame);

        if (pShader != m_pCurrentShader)
        {
            vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                pShader->GetPipeline());
            m_pCurrentShader = pShader;
        }
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

        vkCmdBindDescriptorSets(
            buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pCurrentShader->GetPipelineLayout(),
            0, std::size(m_Sets),
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

        vkCmdEndRenderPass(buffer);
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

    void VulkanRenderer::UpdateDefaultMaterialUniforms()
    {
        for (uint32_t i = 0; i < m_Context.GetFramesInFlight(); i++)
        {
            VulkanMaterial::Uniforms uniforms;
            void* data = m_DefaultMatUniforms->GetMappedData(i);
            *static_cast<VulkanMaterial::Uniforms*>(data) = uniforms;
        }
    }

    void VulkanRenderer::CreateSwapchain(const TetherVulkan::SwapchainDetails& details)
    {
        m_Swapchain.emplace(m_Context, details, m_SurfaceFormat,
            m_Surface.Get(), m_Window.GetWidth(), m_Window.GetHeight(),
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

    void VulkanRenderer::CreateUniforms()
    {
        CreateCameraDescriptorSetLayout();
        CreateMaterialDescriptorSetLayout();
        CreateSceneDescriptorSetLayout();

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

        m_CameraSet.emplace(*m_StaticUniformPool, m_CameraLayout, framesInFlight);
        m_CameraUniforms.emplace(m_Context, sizeof(CameraUniforms), *m_CameraSet,
            0);

        m_DefaultMatSet.emplace(*m_StaticUniformPool, m_MaterialLayout, framesInFlight);
        m_DefaultMatUniforms.emplace(m_Context, sizeof(VulkanMaterial::Uniforms), *m_DefaultMatSet, 0);

        m_SceneSet.emplace(*m_StaticUniformPool, m_SceneLayout, framesInFlight);
        m_SceneUniforms.emplace(m_Context, sizeof(SceneUniforms), *m_SceneSet, 0);

        UpdateDefaultMaterialUniforms();
    }

    void VulkanRenderer::CreateShaders()
    {
        static Shader::Stage solidStages[] =
        {
            { ShaderType::VERTEX, _binary_solid_vert_spv,
                sizeof(_binary_solid_vert_spv) },
                { ShaderType::FRAG, _binary_solid_frag_spv,
                    sizeof(_binary_solid_frag_spv) }
        };

        static Shader::Stage texturedStages[] =
        {
            { ShaderType::VERTEX, _binary_textured_vert_spv,
                sizeof(_binary_textured_vert_spv) },
                { ShaderType::FRAG, _binary_textured_frag_spv,
                    sizeof(_binary_textured_frag_spv) }
        };

        VkDescriptorSetLayout sets[] =
        {
            m_CameraLayout,
            m_MaterialLayout,
        };

        m_SolidShader.emplace(m_Context, solidStages, sets, m_RenderPass);
        m_TexturedShader.emplace(m_Context, texturedStages, sets, m_RenderPass);
    }

    void VulkanRenderer::CreateDepthImages()
    {
        VkExtent2D swapchainExtent = m_Swapchain->GetExtent();

        Tether::Rendering::Resources::BufferedImageInfo info{};
        info.width = swapchainExtent.width;
        info.height = swapchainExtent.height;

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

    void VulkanRenderer::CreateFramebuffers()
    {
        VkExtent2D swapchainExtent = m_Swapchain->GetExtent();
        uint64_t imageViewCount = m_SwapchainImageViews.size();

        m_Framebuffers.resize(imageViewCount);

        for (uint64_t i = 0; i < imageViewCount; i++)
        {
            const VkImageView attachments[] =
            {
                m_SwapchainImageViews[i],
                m_DepthImageView
            };

            VkFramebufferCreateInfo framebufferDesc{};
            framebufferDesc.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferDesc.renderPass = m_RenderPass;
            framebufferDesc.attachmentCount = 2;
            framebufferDesc.pAttachments = attachments;
            framebufferDesc.width = swapchainExtent.width;
            framebufferDesc.height = swapchainExtent.height;
            framebufferDesc.layers = 1;

            if (vkCreateFramebuffer(m_Device, &framebufferDesc, nullptr,
                &m_Framebuffers[i]) != VK_SUCCESS)
                throw std::runtime_error("Failed to create framebuffer");
        }
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

    void VulkanRenderer::CreateCameraDescriptorSetLayout()
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

        if (vkCreateDescriptorSetLayout(m_Device,
            &setInfo, nullptr, &m_CameraLayout) != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor set layout");
    }

    void VulkanRenderer::CreateMaterialDescriptorSetLayout()
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
        setInfo.bindingCount = std::size(bindings);
        setInfo.pBindings = bindings;

        if (vkCreateDescriptorSetLayout(m_Device,
            &setInfo, nullptr, &m_MaterialLayout) != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor set layout");
    }

    void VulkanRenderer::CreateSceneDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding uniformBinding{};
        uniformBinding.binding = 0;
        uniformBinding.descriptorCount = 1;
        uniformBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo setInfo{};
        setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        setInfo.bindingCount = 1;
        setInfo.pBindings = &uniformBinding;

        if (vkCreateDescriptorSetLayout(m_Device,
            &setInfo, nullptr, &m_SceneLayout) != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor set layout");
    }

    TetherVulkan::SwapchainDetails VulkanRenderer::QuerySwapchainSupport()
    {
        TetherVulkan::SwapchainDetails details;

        const VkSurfaceKHR surface = m_Surface.Get();

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            m_PhysicalDevice,
            surface,
            &details.capabilities
        );

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            m_PhysicalDevice,
            surface,
            &formatCount,
            nullptr
        );

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(
                m_PhysicalDevice,
                surface,
                &formatCount,
                details.formats.data()
            );
        }

        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            m_PhysicalDevice,
            surface,
            &presentModeCount,
            nullptr
        );

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                m_PhysicalDevice,
                surface,
                &presentModeCount,
                details.presentModes.data()
            );
        }

        return details;
    }

    void VulkanRenderer::ChooseSurfaceFormat(const TetherVulkan::SwapchainDetails& details)
    {
        for (const auto& availableFormat : details.formats)
            if (availableFormat.format == VK_FORMAT_R32G32B32_UINT)
            {
                m_SurfaceFormat = availableFormat;
                return;
            }

        m_SurfaceFormat = details.formats[0];
    }

    VkFormat VulkanRenderer::FindDepthFormat() const
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

    VkFormat VulkanRenderer::FindSupportedFormat(const std::vector<VkFormat>& candidates,
        const VkImageTiling tiling, const VkFormatFeatureFlags features) const
    {
        for (VkFormat format : candidates)
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
        ChooseSurfaceFormat(details);

        if (details.capabilities.currentExtent.width == 0 ||
            details.capabilities.currentExtent.height == 0)
        {
            m_ShouldRecreateSwapchain = false;
            return;
        }

        DestroySwapchain();

        CreateSwapchain(details);
        CreateDepthImages();
        CreateFramebuffers();
        CreateCommandBuffers();

        m_ShouldRecreateSwapchain = false;
    }

    void VulkanRenderer::DestroySwapchain()
    {
        vkFreeCommandBuffers(m_Device, m_Context.GetCommandPool(),
            static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());

        for (const VkFramebuffer framebuffer : m_Framebuffers)
            vkDestroyFramebuffer(m_Device, framebuffer, nullptr);

        m_DepthImage.reset();

        for (const VkImageView imageView : m_SwapchainImageViews)
            vkDestroyImageView(m_Device, imageView, nullptr);

        m_Swapchain.reset();
    }
}
