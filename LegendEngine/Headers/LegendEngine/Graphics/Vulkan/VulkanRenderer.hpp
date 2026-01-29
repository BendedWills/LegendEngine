#pragma once

#include <LegendEngine/Graphics/Renderer.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanShader.hpp>
#include <Tether/Rendering/Vulkan/DescriptorSet.hpp>
#include <Tether/Rendering/Vulkan/Surface.hpp>

#include <Tether/Rendering/Vulkan/Swapchain.hpp>
#include <Tether/Rendering/Vulkan/UniformBuffer.hpp>
#include <Tether/Rendering/Vulkan/Resources/BufferedImage.hpp>

namespace LegendEngine::Graphics::Vulkan
{
    namespace TetherVulkan = Tether::Rendering::Vulkan;

    class VulkanRenderer final : public Renderer
    {
    public:
        explicit VulkanRenderer(
            Application& app,
            TetherVulkan::GraphicsContext& tetherCtx
        );
        ~VulkanRenderer() override;

        Scope<Resources::Shader> CreateShader(
            std::span<Resources::Shader::Stage> stages) override;
        Scope<Resources::Material> CreateMaterial() override;

        void SetVSyncEnabled(bool vsync) override;
        void NotifyWindowResized() override;
    private:
        struct CameraUniforms
        {
            Matrix4x4f view;
            Matrix4x4f projection;
        };

        bool StartFrame() override;
        void BeginCommandBuffer();
        void UseMaterial(Resources::Material* pMaterial) override;
        void DrawMesh(const Components::MeshComponent& mesh) override;
        void EndFrame() override;

        void UpdateCameraUniforms(const Objects::Camera& camera) override;

        void CreateSwapchain();
        void CreateRenderPass();
        void CreateUniforms();
        void CreateShaders();
        void CreateDepthImages();
        void CreateFramebuffers();
        void CreateCommandBuffers();
        void CreateSyncObjects();

        void CreateCameraDescriptorSetLayout();
        void CreateMaterialDescriptorSetLayout();

        TetherVulkan::SwapchainDetails QuerySwapchainSupport();
        void ChooseSurfaceFormat(const TetherVulkan::SwapchainDetails& details);
        [[nodiscard]] VkFormat FindDepthFormat() const;
        [[nodiscard]] VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates,
            VkImageTiling tiling, VkFormatFeatureFlags features) const;

        void RecreateSwapchain();
        void DestroySwapchain();

        bool m_VSync = false;
        bool m_ShouldRecreateSwapchain = false;

        TetherVulkan::GraphicsContext& m_Context;
        Tether::Window& m_Window;
        TetherVulkan::Surface m_Surface;

        std::optional<TetherVulkan::Swapchain> m_Swapchain;
        VkRenderPass m_RenderPass;
        VkDescriptorSetLayout m_CameraLayout = nullptr;
        VkDescriptorSetLayout m_MaterialLayout = nullptr;
        std::optional<TetherVulkan::DescriptorPool> m_StaticUniformPool;
        std::optional<TetherVulkan::DescriptorSet> m_CameraSet;
        std::optional<TetherVulkan::DescriptorSet> m_DefaultMatSet;
        std::optional<TetherVulkan::UniformBuffer> m_CameraUniforms;
        std::optional<TetherVulkan::UniformBuffer> m_DefaultMatUniforms;

        std::optional<VulkanShader> m_SolidShader;
        std::optional<VulkanShader> m_TexturedShader;

        std::optional<TetherVulkan::BufferedImage> m_DepthImage;
        VkImageView m_DepthImageView;

        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkImageView> m_SwapchainImageViews;
        std::vector<VkFramebuffer> m_Framebuffers;
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;

        VkDevice m_Device;
        VkPhysicalDevice m_PhysicalDevice;

        VkSurfaceFormatKHR m_SurfaceFormat{};

        uint32_t m_CurrentFrame = 0;
        uint32_t m_CurrentImageIndex = 0;

        VkDescriptorSet m_Sets[2] = {};
        VulkanShader* m_pCurrentShader = nullptr;
    };
}
