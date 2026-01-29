#pragma once

#include <LegendEngine/Common/Logger.hpp>
#include <LegendEngine/Graphics/GraphicsContext.hpp>
#include <Tether/Rendering/Vulkan/ContextCreator.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>
#include <Tether/Rendering/Vulkan/Instance.hpp>

namespace LegendEngine::Graphics::Vulkan
{
    namespace TetherVulkan = Tether::Rendering::Vulkan;

    class VulkanGraphicsContext final : public GraphicsContext
    {
    public:
        VulkanGraphicsContext(std::string_view applicationName,
            bool debug, Logger& logger);
        ~VulkanGraphicsContext() override;

        Scope<Renderer> CreateRenderer(Application& app) override;

        Scope<VertexBuffer> CreateVertexBuffer(
            std::span<VertexTypes::Vertex3> vertices,
            std::span<uint32_t> indices) override;
        Scope<Resources::Texture2D> CreateTexture2D(IO::TextureLoader& loader) override;

        TetherVulkan::GraphicsContext& GetTetherGraphicsContext();
    private:
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

        Logger& m_Logger;

        TetherVulkan::ContextCreator m_ContextCreator;
        TetherVulkan::GraphicsContext m_GraphicsContext;

        DebugCallback m_Callback;
    };
}
