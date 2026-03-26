#pragma once

#include <LE/Graphics/GraphicsContext.hpp>
#include <../Done/VulkanShader.hpp>
#include <VulkanShaderManager.hpp>
#include <Tether/Rendering/Vulkan/ContextCreator.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>
#include <Tether/Rendering/Vulkan/Instance.hpp>
#include <Tether/Rendering/Vulkan/UniformBuffer.hpp>

namespace le
{
    namespace TetherVulkan = Tether::Rendering::Vulkan;

    class VulkanGraphicsContext final : public GraphicsContext
    {
    public:
        explicit VulkanGraphicsContext(std::string_view applicationName);
        ~VulkanGraphicsContext() override;

        Scope<Renderer> CreateRenderer(RenderTarget& renderTarget) override;
        Scope<RenderTarget> CreateHeadlessRenderTarget() override;
#ifndef LE_HEADLESS
        Scope<RenderTarget> CreateWindowRenderTarget(Tether::Window& window) override;
#endif

        Scope<VertexBuffer> CreateVertexBuffer(size_t initialVertexCount,
            size_t initialIndexCount, VertexBuffer::UpdateFrequency updateFrequency) override;
        Scope<Texture2D> CreateTexture2D(const TextureData& loader) override;
        Scope<Texture2DArray> CreateTexture2DArray(size_t width,
            size_t height, uint8_t channels,
            const std::span<TextureData*>& textureData) override;
        Scope<Shader> CreateShader(
            std::span<Shader::Stage> stages) override;
        Scope<Material> CreateMaterial() override;

        VkDescriptorSetLayout GetCameraLayout() const;
        VkDescriptorSetLayout GetMaterialLayout() const;
        VkDescriptorSetLayout GetSceneLayout() const;

        std::span<VkDescriptorSetLayout> GetSets();

        const ShaderManager& GetShaderManager() const override;

        VkFormat GetDepthFormat() const;

        std::mutex& GetGraphicsQueueMutex();
        std::mutex& GetTransferQueueMutex() const;
        VkQueue GetTransferQueue() const;
        VkCommandPool GetTransferPool() const;

        TetherVulkan::GraphicsContext& GetTetherGraphicsContext();
    private:
        static TetherVulkan::ContextCreator::Info GetContextInfo(std::string_view applicationName);

        VkFormat FindDepthFormat() const;

        std::mutex& FindTransferMutex();

        void CreateCameraDescriptorSetLayout();
        void CreateSceneDescriptorSetLayout();
        void CreateMaterialDescriptorSetLayout();

        void CreateUniforms();
        void CreateTransferQueue();
        void UpdateDefaultMaterialUniforms();

        class DebugCallback final : public TetherVulkan::DebugCallback
        {
        public:
            explicit DebugCallback(VulkanGraphicsContext& context);

            void OnDebugLog(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
            ) override;
        private:
            VulkanGraphicsContext& m_Context;
        };

        TetherVulkan::ContextCreator m_ContextCreator;
        TetherVulkan::GraphicsContext m_GraphicsContext;

        DebugCallback m_Callback;

        VkDescriptorSetLayout m_CameraLayout = nullptr;
        VkDescriptorSetLayout m_SceneLayout = nullptr;
        VkDescriptorSetLayout m_MaterialLayout = nullptr;

        VkSurfaceFormatKHR m_SurfaceFormat{};

        std::optional<TetherVulkan::DescriptorPool> m_StaticUniformPool;
        std::optional<TetherVulkan::DescriptorSet> m_CameraSet;
        std::optional<TetherVulkan::DescriptorSet> m_DefaultMatSet;
        std::optional<TetherVulkan::DescriptorSet> m_SceneSet;
        std::optional<TetherVulkan::UniformBuffer> m_CameraUniforms;
        std::optional<TetherVulkan::UniformBuffer> m_DefaultMatUniforms;
        std::optional<TetherVulkan::UniformBuffer> m_SceneUniforms;

        std::vector<VkDescriptorSetLayout> m_SetLayouts;

        std::optional<VulkanShaderManager> m_ShaderManager;

        std::mutex m_GraphicsQueueMutex;
        std::mutex m_TransferQueueMutex;
        std::mutex& m_ActualTransferMutex;

        VkQueue m_TransferQueue = nullptr;
        VkCommandPool m_TransferPool = nullptr;

        VkFormat m_DepthFormat;
    };
}
