#include <LegendEngine/Graphics/Vulkan/VkDefs.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanGraphicsContext.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanMaterial.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderTarget.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanShader.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanTexture2D.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanVertexBuffer.hpp>
#include <LegendEngine/IO/Logger.hpp>

namespace le
{
    static const char* EXTENSIONS[] =
        { VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME };
    static constexpr VkPhysicalDeviceDynamicRenderingFeaturesKHR DYNAMIC_RENDERING
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
        .dynamicRendering = true
    };

    VulkanGraphicsContext::VulkanGraphicsContext(const std::string_view applicationName)
        :
        m_ContextCreator(GetContextInfo(applicationName)),
        m_GraphicsContext(m_ContextCreator),
        m_Callback(*this),
        m_ActualTransferMutex(FindTransferMutex())
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

        CreateUniforms();
        CreateTransferQueue();

        m_DepthFormat = FindDepthFormat();
        m_ShaderManager.emplace(m_GraphicsContext, m_SetLayouts, m_DepthFormat);

        VkFenceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
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
            m_CameraLayout, m_SceneLayout, surfaceFormat, *m_DefaultMatSet,
            m_DepthFormat
        );
    }

    Scope<RenderTarget> VulkanGraphicsContext::CreateHeadlessRenderTarget()
    {
        return std::make_unique<VulkanRenderTarget>(m_GraphicsContext);
    }

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

        Logger::GetGlobalLogger().Log(level, ss.str());
    }

#ifndef LE_HEADLESS
    Scope<RenderTarget> VulkanGraphicsContext::CreateWindowRenderTarget(Tether::Window& window)
    {
        return std::make_unique<VulkanRenderTarget>(m_GraphicsContext, window);
    }
#endif

    Scope<VertexBuffer> VulkanGraphicsContext::CreateVertexBuffer(
        size_t initialVertexCount, size_t initialIndexCount)
    {
        return std::make_unique<VulkanVertexBuffer>(*this, initialVertexCount, initialIndexCount);
    }

    Scope<Texture2D> VulkanGraphicsContext::CreateTexture2D(const TextureData& loader)
    {
        return std::make_unique<VulkanTexture2D>(m_GraphicsContext, loader);
    }

    Scope<Shader> VulkanGraphicsContext::CreateShader(std::span<Shader::Stage> stages)
    {
        return std::make_unique<VulkanShader>(m_GraphicsContext, stages, m_SetLayouts, m_DepthFormat);
    }

    Scope<Material> VulkanGraphicsContext::CreateMaterial()
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

    void VulkanGraphicsContext::CreateUniforms()
    {
        const uint32_t framesInFlight = m_GraphicsContext.GetFramesInFlight();

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

        m_StaticUniformPool.emplace(m_GraphicsContext, framesInFlight * 3,
            std::size(sizes), sizes);

        m_CameraSet.emplace(*m_StaticUniformPool, m_CameraLayout, framesInFlight);
        m_CameraUniforms.emplace(m_GraphicsContext, sizeof(Camera::CameraUniforms), *m_CameraSet,
            0);

        m_DefaultMatSet.emplace(*m_StaticUniformPool, m_MaterialLayout, framesInFlight);
        m_DefaultMatUniforms.emplace(m_GraphicsContext, sizeof(VulkanMaterial::Uniforms), *m_DefaultMatSet, 0);

        m_SceneSet.emplace(*m_StaticUniformPool, m_SceneLayout, framesInFlight);

        UpdateDefaultMaterialUniforms();
    }

    void VulkanGraphicsContext::UpdateDefaultMaterialUniforms()
    {
        for (uint32_t i = 0; i < m_GraphicsContext.GetFramesInFlight(); i++)
        {
            VulkanMaterial::Uniforms uniforms;
            void* data = m_DefaultMatUniforms->GetMappedData(i);
            *static_cast<VulkanMaterial::Uniforms*>(data) = uniforms;
        }
    }

    VkFormat VulkanGraphicsContext::FindDepthFormat() const
    {
        constexpr VkFormat candidates[] = {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
        };

        constexpr VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

        for (const VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(m_GraphicsContext.GetPhysicalDevice(), format, &props);

            if ((props.optimalTilingFeatures & features) == features)
                return format;
        }

        return candidates[0];
    }

    VkFormat VulkanGraphicsContext::GetDepthFormat() const
    {
        return m_DepthFormat;
    }

    TetherVulkan::ContextCreator::Info VulkanGraphicsContext::GetContextInfo(std::string_view applicationName)
    {
        TetherVulkan::ContextCreator::Info info;
        info.deviceExtensions = EXTENSIONS;
        info.applicationName = applicationName;
        info.engineName = "LegendEngine";
        info.devicePNext = &DYNAMIC_RENDERING;
        info.deviceExtensions = EXTENSIONS;
        info.createTransferQueue = true;

        return info;
    }

    std::mutex& VulkanGraphicsContext::GetGraphicsQueueMutex()
    {
        return m_GraphicsQueueMutex;
    }

    std::mutex& VulkanGraphicsContext::GetTransferQueueMutex() const
    {
        return m_ActualTransferMutex;
    }

    std::mutex& VulkanGraphicsContext::FindTransferMutex()
    {
        const Tether::Rendering::Vulkan::QueueFamilyIndices indices = m_ContextCreator.GetQueueFamilyIndices();
        return indices.graphicsFamilyIndex == indices.transferFamilyIndex ? m_GraphicsQueueMutex : m_TransferQueueMutex;
    }

    VkQueue VulkanGraphicsContext::GetTransferQueue() const
    {
        return m_TransferQueue;
    }

    VkCommandPool VulkanGraphicsContext::GetTransferPool() const
    {
        return m_TransferPool;
    }

    void VulkanGraphicsContext::CreateTransferQueue()
    {
        const Tether::Rendering::Vulkan::QueueFamilyIndices indices = m_ContextCreator.GetQueueFamilyIndices();
        if (indices.graphicsFamilyIndex == indices.transferFamilyIndex)
        {
            m_TransferQueue = m_ContextCreator.GetQueue();
            m_TransferPool = m_ContextCreator.GetCommandPool();
            return;
        }

        m_TransferQueue = m_ContextCreator.GetDeviceQueue(indices.transferFamilyIndex, 0);

        VkCommandPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        info.queueFamilyIndex = indices.transferFamilyIndex;

        LE_CHECK_VK(vkCreateCommandPool(m_ContextCreator.GetDevice(), &info, nullptr, &m_TransferPool));
    }
}
