#pragma once

#include <LE/Common/Defs.hpp>
#include <LE/Math/Types.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>

#include <vulkan/vulkan.h>

namespace le
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
            VkFormat depthFormat;
        };

        struct ObjectTransform
        {
            Matrix4x4f transform;
        };

        Pipeline(Tether::Rendering::Vulkan::GraphicsContext& context,
            const Info& info);
        ~Pipeline();
        LE_NO_COPY(Pipeline);

        [[nodiscard]] VkPipeline Get() const;
        [[nodiscard]] VkPipelineLayout GetPipelineLayout() const;
    private:
        Tether::Rendering::Vulkan::GraphicsContext& m_Context;

        VkPipelineLayout m_PipelineLayout = nullptr;
        VkPipeline m_Pipeline = nullptr;
    };
}