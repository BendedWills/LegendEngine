#include <LegendEngine/Graphics/Vulkan/VulkanGraphicsContext.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanMaterial.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanShader.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanTexture2D.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanVertexBuffer.hpp>

namespace LegendEngine::Graphics::Vulkan
{
    VulkanGraphicsContext::DebugCallback::DebugCallback(VulkanGraphicsContext& context)
        :
        m_Context(context)
    {}

    void VulkanGraphicsContext::DebugCallback::OnDebugLog(
        const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
    )
    {
        std::stringstream ss;
        ss << "Vulkan Validation Layer: " << pCallbackData->pMessage;

        Logger::Level level;
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                level = Logger::Level::INFO;
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                level = Logger::Level::WARN;
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                level = Logger::Level::ERROR;
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                level = Logger::Level::DEBUG;
                break;
            default: level = Logger::Level::INFO;
        }

        m_Context.m_Logger.Log(level, ss.str());

    }

    VulkanGraphicsContext::VulkanGraphicsContext(const std::string_view applicationName,
        const bool debug, Logger& logger)
        :
        m_Logger(logger),
        m_ContextCreator(debug, applicationName, "LegendEngine"),
        m_GraphicsContext(m_ContextCreator),
        m_Callback(*this)
    {
        m_ContextCreator.AddDebugMessenger(&m_Callback);
    }

    VulkanGraphicsContext::~VulkanGraphicsContext()
    {
        m_ContextCreator.RemoveDebugMessenger(&m_Callback);
    }

    Scope<Renderer> VulkanGraphicsContext::CreateRenderer(Application& app)
    {
        return std::make_unique<VulkanRenderer>(app, m_GraphicsContext);
    }

    Scope<VertexBuffer> VulkanGraphicsContext::CreateVertexBuffer(
            std::span<VertexTypes::Vertex3> vertices,
            std::span<uint32_t> indices
        )
    {
        return std::make_unique<VulkanVertexBuffer>(m_GraphicsContext, vertices, indices);
    }

    Scope<Resources::Texture2D> VulkanGraphicsContext::CreateTexture2D(IO::TextureLoader& loader)
    {
        return std::make_unique<VulkanTexture2D>(m_GraphicsContext, loader);
    }

    TetherVulkan::GraphicsContext& VulkanGraphicsContext::GetTetherGraphicsContext()
    {
        return m_GraphicsContext;
    }
}
