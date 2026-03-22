#pragma once

#include <LE/Components/Light.hpp>
#include <LE/Graphics/Renderer.hpp>
#include <../Done/VulkanShader.hpp>
#include <VulkanVertexBuffer.hpp>
#include <Tether/Rendering/Vulkan/DescriptorSet.hpp>
#include <Tether/Rendering/Vulkan/Surface.hpp>

#include <Tether/Rendering/Vulkan/Swapchain.hpp>
#include <Tether/Rendering/Vulkan/UniformBuffer.hpp>
#include <Tether/Rendering/Vulkan/Resources/BufferedImage.hpp>

namespace le
{
    namespace TetherVulkan = Tether::Rendering::Vulkan;

    class VulkanRenderer final : public Renderer
    {
    public:
        explicit VulkanRenderer(
            TetherVulkan::GraphicsContext& tetherCtx,
            RenderTarget& renderTarget,
            ShaderManager& shaderManager,
            VkDescriptorSetLayout cameraLayout,
            VkDescriptorSetLayout sceneLayout,
            VkSurfaceFormatKHR surfaceFormat,
            TetherVulkan::DescriptorSet& defaultMatSet,
            VkFormat depthFormat,
            std::mutex& graphicsQueueMutex
        );
        ~VulkanRenderer() override;

        void SetVSyncEnabled(bool vsync) override;
        void NotifyWindowResized() override;
    private:
        struct LightUniforms
        {
            Color color;
            alignas(16) Vector3f position;
            Light::LightType type;
        };

        struct SceneUniforms
        {
            Color ambientLight;
            alignas(16) LightUniforms lights[8];
        };

        bool StartFrame() override;
        void BeginCommandBuffer();
        void UseMaterial(Material* pMaterial) override;
        void DrawMesh(const Mesh& mesh) override;
        void EndCommandBuffer() const;
        void EndFrame() override;

        void UpdateCameraUniforms(const Camera& camera) override;

        void CreateSwapchain(const TetherVulkan::SwapchainDetails& details);
        void CreateUniforms(VkDescriptorSetLayout cameraLayout,
            VkDescriptorSetLayout sceneLayout);
        void CreateDepthImages();
        void CreateCommandBuffers();
        void CreateSyncObjects();

        TetherVulkan::SwapchainDetails QuerySwapchainSupport() const;
        [[nodiscard]] VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates,
            VkImageTiling tiling, VkFormatFeatureFlags features) const;

        void RecreateSwapchain();
        void DestroySwapchain();

        bool m_VSync = false;
        bool m_ShouldRecreateSwapchain = false;

        TetherVulkan::GraphicsContext& m_Context;
        const TetherVulkan::DeviceLoader& m_DeviceLoader;
        TetherVulkan::DescriptorSet& m_DefaultMatSet;
        ShaderManager& m_ShaderManager;
        VkSurfaceKHR m_Surface;

        std::optional<TetherVulkan::Swapchain> m_Swapchain;
        std::optional<TetherVulkan::DescriptorPool> m_StaticUniformPool;
        std::optional<TetherVulkan::DescriptorSet> m_CameraSet;
        std::optional<TetherVulkan::DescriptorSet> m_SceneSet;
        std::optional<TetherVulkan::UniformBuffer> m_CameraUniforms;
        std::optional<TetherVulkan::UniformBuffer> m_SceneUniforms;

        std::vector<VkImage> m_DepthImages;
        std::vector<VmaAllocation> m_DepthAllocs;
        std::vector<VkImageView> m_DepthImageViews;

        uint32_t m_SwapchainImageCount = 0;
        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkImageView> m_SwapchainImageViews;
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;

        VkSurfaceFormatKHR m_SurfaceFormat;

        Ref<VulkanVertexBuffer> m_WaitingForStagerDeletion = nullptr;
        uint32_t m_FrameWithStagerWaits = 0;
        size_t m_StagerSemaphoreValue = 0;
        bool m_ShouldWaitForStager = false;
        bool m_IsFrameWaiting = false;

        VkDevice m_Device;
        VkPhysicalDevice m_PhysicalDevice;

        uint32_t m_CurrentFrame = 0;
        uint32_t m_CurrentImageIndex = 0;

        VkFormat m_DepthFormat;

        VkDescriptorSet m_Sets[3] = {};
        bool m_HaveSetsChanged = true;
        VulkanShader* m_pCurrentShader = nullptr;

        std::mutex& m_GraphicsQueueMutex;
    };
}
