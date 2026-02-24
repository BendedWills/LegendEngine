#include <LegendEngine/Graphics/Vulkan/VulkanGraphicsContext.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanMaterial.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderTargetBridge.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanShader.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanTexture2D.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanVertexBuffer.hpp>

namespace LegendEngine::Graphics::Vulkan
{
    static const char* EXTENTIONS[] =
        { VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME };
    static constexpr VkPhysicalDeviceDynamicRenderingFeaturesKHR DYNAMIC_RENDERING
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
        .dynamicRendering = true
    };

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
        const bool debug)
        :
        m_Logger("VulkanGraphicsContext", true, debug),
        m_ContextCreator(debug, applicationName, "LegendEngine",
            EXTENTIONS, &DYNAMIC_RENDERING),
        m_GraphicsContext(m_ContextCreator),
        m_Callback(*this)
    {
        m_ContextCreator.AddDebugMessenger(&m_Callback);

        if (const TetherVulkan::DeviceLoader& loader = m_GraphicsContext.GetDeviceLoader();
            !loader.vkCmdBeginRenderingKHR || !loader.vkCmdEndRenderingKHR)
            throw std::runtime_error("Couldn't load dynamic rendering funcs");

        // This order matters, because the sets get added to m_SetLayouts when
        // they are created, and Vulkan cares about the order in vkCmdBindDescriptorSets
        CreateCameraDescriptorSetLayout();
        CreateSceneDescriptorSetLayout();
        CreateMaterialDescriptorSetLayout();

        m_ShaderManager.emplace(m_GraphicsContext, m_SetLayouts);
    }

    VulkanGraphicsContext::~VulkanGraphicsContext()
    {
        m_ContextCreator.RemoveDebugMessenger(&m_Callback);

        m_ShaderManager.reset();

        vkDestroyDescriptorSetLayout(m_GraphicsContext.GetDevice(), m_CameraLayout, nullptr);
        vkDestroyDescriptorSetLayout(m_GraphicsContext.GetDevice(), m_MaterialLayout, nullptr);
        vkDestroyDescriptorSetLayout(m_GraphicsContext.GetDevice(), m_SceneLayout, nullptr);
    }

    Scope<Renderer> VulkanGraphicsContext::CreateRenderer(RenderTarget& renderTarget)
    {
        constexpr VkSurfaceFormatKHR surfaceFormat =
        {
            .format = VK_FORMAT_B8G8R8A8_UNORM,
            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        };

        return std::make_unique<VulkanRenderer>(
            m_GraphicsContext, renderTarget, *m_ShaderManager,
            m_CameraLayout, m_SceneLayout, surfaceFormat, *m_DefaultMatSet
        );
    }

    Scope<RenderTargetBridge> VulkanGraphicsContext::CreateHeadlessRenderTargetBridge()
    {
        return std::make_unique<VulkanRenderTargetBridge>(m_GraphicsContext);
    }

#ifndef LGENG_HEADLESS
    Scope<RenderTargetBridge> VulkanGraphicsContext::CreateWindowRenderTargetBridge(Tether::Window& window)
    {
        return std::make_unique<VulkanRenderTargetBridge>(m_GraphicsContext, window);
    }
#endif

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

    Scope<Resources::Shader> VulkanGraphicsContext::CreateShader(std::span<Resources::Shader::Stage> stages)
    {
        return std::make_unique<VulkanShader>(m_GraphicsContext, stages, m_SetLayouts);
    }

    Scope<Resources::Material> VulkanGraphicsContext::CreateMaterial()
    {
        return std::make_unique<VulkanMaterial>(m_GraphicsContext, m_MaterialLayout);
    }

    TetherVulkan::GraphicsContext& VulkanGraphicsContext::GetTetherGraphicsContext()
    {
        return m_GraphicsContext;
    }

    VkDescriptorSetLayout VulkanGraphicsContext::GetCameraLayout() const
    {
        return m_CameraLayout;
    }

    VkDescriptorSetLayout VulkanGraphicsContext::GetMaterialLayout() const
    {
        return m_MaterialLayout;
    }

    VkDescriptorSetLayout VulkanGraphicsContext::GetSceneLayout() const
    {
        return m_SceneLayout;
    }

    std::span<VkDescriptorSetLayout> VulkanGraphicsContext::GetSets()
    {
        return m_SetLayouts;
    }

    const ShaderManager& VulkanGraphicsContext::GetShaderManager() const
    {
        return *m_ShaderManager;
    }

    void VulkanGraphicsContext::CreateCameraDescriptorSetLayout()
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

        if (vkCreateDescriptorSetLayout(m_GraphicsContext.GetDevice(),
            &setInfo, nullptr, &m_CameraLayout) != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor set layout");

        m_SetLayouts.push_back(m_CameraLayout);
    }

    void VulkanGraphicsContext::CreateMaterialDescriptorSetLayout()
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

        if (vkCreateDescriptorSetLayout(m_GraphicsContext.GetDevice(),
            &setInfo, nullptr, &m_MaterialLayout) != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor set layout");

        m_SetLayouts.push_back(m_MaterialLayout);
    }

    void VulkanGraphicsContext::CreateSceneDescriptorSetLayout()
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

        if (vkCreateDescriptorSetLayout(m_GraphicsContext.GetDevice(),
            &setInfo, nullptr, &m_SceneLayout) != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor set layout");

        m_SetLayouts.push_back(m_SceneLayout);
    }
}
