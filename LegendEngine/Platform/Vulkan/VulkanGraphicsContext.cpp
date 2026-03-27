#include "VulkanGraphicsContext.hpp"

#include "API/Buffer.hpp"
#include "API/CommandBuffer.hpp"
#include "API/DynamicUniforms.hpp"
#include "API/Image.hpp"
#include "API/ImageView.hpp"
#include "API/Pipeline.hpp"
#include "API/Sampler.hpp"

#include "solid.frag.spv.h"
#include "solid.vert.spv.h"
#include "textured.frag.spv.h"
#include "textured.vert.spv.h"

#include <Renderer.hpp>
#include <ShaderModule.hpp>
#include <VkDefs.hpp>
#include <VulkanRenderTarget.hpp>
#include <LE/Application.hpp>
#include <LE/Common/Assert.hpp>
#include <LE/IO/Logger.hpp>

namespace le::vk
{
    static const char* EXTENSIONS[] =
    {
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
    };

    static VkPhysicalDeviceSynchronization2FeaturesKHR SYNCHRONIZATION_2
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR,
        .synchronization2 = true
    };

    static VkPhysicalDeviceTimelineSemaphoreFeaturesKHR TIMELINE_SEMAPHORE
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR,
        .pNext = static_cast<void*>(&SYNCHRONIZATION_2),
        .timelineSemaphore = true,
    };

    static constexpr VkPhysicalDeviceDynamicRenderingFeaturesKHR DYNAMIC_RENDERING
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
        .pNext = static_cast<void*>(&TIMELINE_SEMAPHORE),
        .dynamicRendering = true,
    };

    Scope<GraphicsContext> CreateVulkanGraphicsContext(std::string_view applicationName)
    {
        return std::make_unique<VulkanGraphicsContext>(applicationName);
    }

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

        VkFenceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }

    VulkanGraphicsContext::~VulkanGraphicsContext()
    {
        m_ContextCreator.RemoveDebugMessenger(&m_Callback);

        vkDestroyDescriptorSetLayout(m_GraphicsContext.GetDevice(), m_CameraLayout, nullptr);
        vkDestroyDescriptorSetLayout(m_GraphicsContext.GetDevice(), m_MaterialLayout, nullptr);
        vkDestroyDescriptorSetLayout(m_GraphicsContext.GetDevice(), m_SceneLayout, nullptr);
    }

    Scope<le::Renderer> VulkanGraphicsContext::CreateRenderer(RenderTarget& renderTarget)
    {
        constexpr VkSurfaceFormatKHR surfaceFormat =
        {
            .format = VK_FORMAT_B8G8R8A8_SRGB,
            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        };

        return std::make_unique<Renderer>(
            *this, renderTarget, surfaceFormat
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

    VkFormat VulkanGraphicsContext::GetDepthFormat() const
    {
        return m_DepthFormat;
    }

    std::mutex& VulkanGraphicsContext::GetGraphicsQueueMutex()
    {
        return m_GraphicsQueueMutex;
    }

    std::mutex& VulkanGraphicsContext::GetTransferQueueMutex() const
    {
        return m_ActualTransferMutex;
    }

    VkQueue VulkanGraphicsContext::GetTransferQueue() const
    {
        return m_TransferQueue;
    }

    VkCommandPool VulkanGraphicsContext::GetTransferPool() const
    {
        return m_TransferPool;
    }

    TetherVulkan::GraphicsContext& VulkanGraphicsContext::GetTetherGraphicsContext()
    {
        return m_GraphicsContext;
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

    std::mutex& VulkanGraphicsContext::FindTransferMutex()
    {
        const Tether::Rendering::Vulkan::QueueFamilyIndices indices = m_ContextCreator.GetQueueFamilyIndices();
        return indices.graphicsFamilyIndex == indices.transferFamilyIndex ? m_GraphicsQueueMutex : m_TransferQueueMutex;
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
        m_DefaultMatUniforms.emplace(m_GraphicsContext, sizeof(Material::Uniforms), *m_DefaultMatSet, 0);

        m_SceneSet.emplace(*m_StaticUniformPool, m_SceneLayout, framesInFlight);
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

    Scope<le::Buffer> VulkanGraphicsContext::CreateSimpleBuffer(Buffer::Usage usage, size_t size, bool createMapped)
    {

    }

    Scope<le::Buffer> VulkanGraphicsContext::CreateSmartBuffer(Buffer::Usage usage, size_t initialSize)
    {

    }

    Scope<le::Buffer> VulkanGraphicsContext::CreatePerFrameBuffer(Buffer::Usage usage, size_t size)
    {

    }

    Scope<le::CommandBuffer> VulkanGraphicsContext::CreateCommandBuffer(const bool transfer)
    {
        if (transfer)
            return std::make_unique<CommandBuffer>(m_TransferQueue, m_TransferQueueMutex);

        return std::make_unique<CommandBuffer>(m_GraphicsContext.GetQueue(), m_GraphicsQueueMutex);
    }

    Scope<le::DynamicUniforms> VulkanGraphicsContext::CreateDynamicUniforms(
        std::span<DynamicUniforms::DescriptorInfo> infos)
    {
        return std::make_unique<DynamicUniforms>();
    }

    Scope<le::Pipeline> VulkanGraphicsContext::CreatePipeline(std::span<Shader::Stage> stages)
    {
        std::vector<VkPipelineShaderStageCreateInfo> vkStages;
        std::vector<ShaderModule> shaderModules;
        vkStages.reserve(stages.size());
        shaderModules.reserve(stages.size());

        for (Shader::Stage stage : stages)
        {
            shaderModules.emplace_back(m_GraphicsContext, stage);

            VkPipelineShaderStageCreateInfo vkStage{};
            vkStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vkStage.module = shaderModules.back().Get();
            vkStage.pName = "main";

            switch (stage.type)
            {
                case ShaderType::VERTEX: vkStage.stage = VK_SHADER_STAGE_VERTEX_BIT; break;
                case ShaderType::FRAG:  vkStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
                case ShaderType::COMPUTE: vkStage.stage = VK_SHADER_STAGE_COMPUTE_BIT; break;
                case ShaderType::GEOM:  vkStage.stage = VK_SHADER_STAGE_GEOMETRY_BIT; break;
            }

            vkStages.push_back(vkStage);
        }

        VkDynamicState dynamicStates[] =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_CULL_MODE
        };

        Pipeline::Info pipelineInfo{};
        pipelineInfo.stageCount = vkStages.size();
        pipelineInfo.pStages = vkStages.data();
        pipelineInfo.pDynamicStates = dynamicStates;
        pipelineInfo.dynamicStateCount = std::size(dynamicStates);
        pipelineInfo.pDynamicStates = dynamicStates;
        pipelineInfo.setCount = m_SetLayouts.size();
        pipelineInfo.pSetLayouts = m_SetLayouts.data();
        pipelineInfo.depthFormat = m_DepthFormat;

        return std::make_unique<Pipeline>(m_GraphicsContext, pipelineInfo);
    }

    Scope<le::Image> VulkanGraphicsContext::CreateImage(const Image::Info& info)
    {
        return std::make_unique<Image>();
    }

    Scope<le::ImageView> VulkanGraphicsContext::CreateImageView(const ImageView::Info& info)
    {
        return std::make_unique<ImageView>();
    }

    Scope<le::Sampler> VulkanGraphicsContext::CreateSampler(const Sampler::Info& info)
    {
        return std::make_unique<Sampler>();
    }

#define STAGES(id) \
    static Shader::Stage STAGES_##id[] = { \
        { ShaderType::VERTEX, _binary_##id##_vert_spv, \
            sizeof(_binary_##id##_vert_spv) }, \
        { ShaderType::FRAG, _binary_##id##_frag_spv, \
            sizeof(_binary_##id##_frag_spv) } \
    }

    STAGES(solid);
    STAGES(textured);

    void VulkanGraphicsContext::RegisterShaders()
    {
        ResourceManager& manager = Application::Get().GetResourceManager();

        m_shaderManager.RegisterShader("solid", manager.CreateResource<Shader>(std::span(STAGES_solid))->id);
        m_shaderManager.RegisterShader("textured", manager.CreateResource<Shader>(std::span(STAGES_textured))->id);
    }
}
