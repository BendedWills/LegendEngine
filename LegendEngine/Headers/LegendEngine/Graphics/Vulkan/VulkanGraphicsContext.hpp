#pragma once

#include <LegendEngine/Graphics/GraphicsContext.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanShader.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanShaderManager.hpp>
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
        VulkanGraphicsContext(std::string_view applicationName);
        ~VulkanGraphicsContext() override;

        Scope<Renderer> CreateRenderer(RenderTarget& renderTarget) override;
        Scope<RenderTarget> CreateHeadlessRenderTarget() override;
#ifndef LE_HEADLESS
        Scope<RenderTarget> CreateWindowRenderTarget(Tether::Window& window) override;
#endif

        Scope<VertexBuffer> CreateVertexBuffer(size_t initialVertexCount,
            size_t initialIndexCount) override;
        Scope<Texture2D> CreateTexture2D(const TextureData& loader) override;
        Scope<Shader> CreateShader(
            std::span<Shader::Stage> stages) override;
        Scope<Material> CreateMaterial() override;

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
