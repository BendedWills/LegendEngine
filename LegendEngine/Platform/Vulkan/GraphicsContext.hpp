#pragma once

#include <LE/Graphics/GraphicsContext.hpp>
#include <Tether/Rendering/Vulkan/ContextCreator.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>
#include <Tether/Rendering/Vulkan/Instance.hpp>
#include <Tether/Rendering/Vulkan/UniformBuffer.hpp>

namespace le::vk
{
    namespace TetherVulkan = Tether::Rendering::Vulkan;

    class GraphicsContext final : public le::GraphicsContext
    {
    public:
        explicit GraphicsContext(std::string_view applicationName);
        ~GraphicsContext() override;

        Scope<Renderer> CreateRenderer(RenderTarget& renderTarget) override;
        Scope<RenderTarget> CreateHeadlessRenderTarget() override;
#ifndef LE_HEADLESS
        Scope<RenderTarget> CreateWindowRenderTarget(Tether::Window& window) override;
#endif
        Scope<Buffer> CreateSimpleBuffer(Buffer::Usage usage, size_t size, bool createMapped) override;
        Scope<Buffer> CreateSmartBuffer(Buffer::Usage usage) override;
        Scope<Buffer> CreatePerFrameBuffer(Buffer::Usage usage, size_t size) override;
        Scope<CommandBuffer> CreateCommandBuffer(bool transfer) override;
        Scope<DescriptorSetLayout> CreateDescriptorSetLayout(std::span<DescriptorSetLayout::Binding> bindings) override;
        Scope<DynamicUniforms> CreateDynamicUniforms(std::span<DynamicUniforms::DescriptorInfo> infos) override;
        Scope<Pipeline> CreatePipeline(std::span<Shader::Stage> stages) override;
        Scope<Image> CreateImage(const Image::Info& info) override;
        Scope<Sampler> CreateSampler(const Sampler::Info& info) override;

        VkQueue GetTransferQueue() const;
        VkCommandPool GetTransferPool() const;
        VkDescriptorSetLayout GetCameraLayout() const;
        VkDescriptorSetLayout GetMaterialLayout() const;
        VkDescriptorSetLayout GetSceneLayout() const;
        std::span<VkDescriptorSetLayout> GetSets();
        VkFormat GetDepthFormat() const;

        std::mutex& GetGraphicsQueueMutex();
        std::mutex& GetTransferQueueMutex() const;

        TetherVulkan::GraphicsContext& GetTetherGraphicsContext();
    private:
        static TetherVulkan::ContextCreator::Info GetContextInfo(std::string_view applicationName);

        VkFormat FindDepthFormat() const;

        std::mutex& FindTransferMutex();

        void CreateCameraDescriptorSetLayout();
        void CreateSceneDescriptorSetLayout();
        void CreateMaterialDescriptorSetLayout();
        void RegisterShaders(ShaderManager& shaderManager) override;
        void CreateUniforms();
        void CreateTransferQueue();

        class DebugCallback final : public TetherVulkan::DebugCallback
        {
        public:
            explicit DebugCallback(GraphicsContext& context);

            void OnDebugLog(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
            ) override;
        private:
            GraphicsContext& m_Context;
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

        std::mutex m_GraphicsQueueMutex;
        std::mutex m_TransferQueueMutex;
        std::mutex& m_ActualTransferMutex;

        VkQueue m_TransferQueue = nullptr;
        VkCommandPool m_TransferPool = nullptr;

        VkFormat m_DepthFormat;
    };
}
