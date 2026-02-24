#pragma once

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Math/Types.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>

#include <vulkan/vulkan.h>

namespace LegendEngine::Graphics::Vulkan
{
    class Pipeline final
    {
    public:
        struct Info
        {
            uint32_t stageCount;
            VkPipelineShaderStageCreateInfo* pStages;
            uint32_t setCount;
            VkDescriptorSetLayout* pSetLayouts;
            uint32_t dynamicStateCount;
            VkDynamicState* pDynamicStates;
            VkSurfaceFormatKHR surfaceFormat;
        };

        struct ObjectTransform
        {
            Matrix4x4f transform;
        };

        Pipeline(Tether::Rendering::Vulkan::GraphicsContext& context,
            const Info& info);
        ~Pipeline();
        LEGENDENGINE_NO_COPY(Pipeline);

        VkPipeline Get() const;
        VkPipelineLayout GetPipelineLayout() const;
    private:
        Tether::Rendering::Vulkan::GraphicsContext& m_Context;

        VkPipelineLayout m_PipelineLayout = nullptr;
        VkPipeline m_Pipeline = nullptr;
    };
}