#pragma once

#include <LegendEngine/Graphics/GraphicsContext.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanShader.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanShaderManager.hpp>
#include <Tether/Rendering/Vulkan/ContextCreator.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>
#include <Tether/Rendering/Vulkan/Instance.hpp>
#include <Tether/Rendering/Vulkan/UniformBuffer.hpp>

namespace LegendEngine::Graphics::Vulkan
{
    namespace TetherVulkan = Tether::Rendering::Vulkan;

    class VulkanGraphicsContext final : public GraphicsContext
    {
    public:
        VulkanGraphicsContext(std::string_view applicationName);
        ~VulkanGraphicsContext() override;

        Scope<Renderer> CreateRenderer(RenderTarget& renderTarget) override;
        Scope<RenderTargetBridge> CreateHeadlessRenderTargetBridge() override;
#ifndef LGENG_HEADLESS
        Scope<RenderTargetBridge> CreateWindowRenderTargetBridge(Tether::Window& window) override;
#endif

        Scope<VertexBuffer> CreateVertexBuffer(
            std::span<VertexTypes::Vertex3> vertices,
            std::span<uint32_t> indices) override;
        Scope<Resources::Texture2D> CreateTexture2D(IO::TextureLoader& loader) override;
        Scope<Resources::Shader> CreateShader(
            std::span<Resources::Shader::Stage> stages) override;
        Scope<Resources::Material> CreateMaterial() override;

        VkDescriptorSetLayout GetCameraLayout() const;
        VkDescriptorSetLayout GetMaterialLayout() const;
        VkDescriptorSetLayout GetSceneLayout() const;

        std::span<VkDescriptorSetLayout> GetSets();

        const ShaderManager& GetShaderManager() const override;

        VkFormat GetDepthFormat() const;

        TetherVulkan::GraphicsContext& GetTetherGraphicsContext();
    private:
        VkFormat FindDepthFormat() const;

        void CreateCameraDescriptorSetLayout();
        void CreateSceneDescriptorSetLayout();
        void CreateMaterialDescriptorSetLayout();

        void CreateUniforms();
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

        bool m_Debug = false;

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

        VkFormat m_DepthFormat;
    };
}
