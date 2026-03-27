#pragma once

#include <LE/Graphics/API/DynamicUniforms.hpp>

#include "GraphicsContext.hpp"

namespace le::vk
{
    class DynamicUniforms final : public le::DynamicUniforms
    {
    public:
        explicit DynamicUniforms(GraphicsContext& context, std::span<DescriptorInfo> infos);
        ~DynamicUniforms() override;

        void UpdateBuffer(le::Buffer& buffer, uint32_t binding) override;
        void UpdateSampledImage(uint32_t binding) override;
        void UpdateSampler(uint32_t binding) override;
        void UpdateCombinedImageSampler(uint32_t binding) override;

        void Invalidate() override;
    private:
        TetherVulkan::GraphicsContext& m_context;

        VkDescriptorPool m_descriptorPool = nullptr;

        size_t m_framesUntilValid = 0;
    };
}
