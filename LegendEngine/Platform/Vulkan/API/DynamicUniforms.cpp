#include "API/DynamicUniforms.hpp"

namespace le::vk
{
    DynamicUniforms::DynamicUniforms(GraphicsContext& context, std::span<DescriptorInfo> infos)
        :
        m_context(context.GetTetherGraphicsContext())
    {

    }

    DynamicUniforms::~DynamicUniforms()
    {
        vkDestroyDescriptorPool(m_context.GetDevice(), m_descriptorPool, nullptr);
    }

    void DynamicUniforms::UpdateBuffer(le::Buffer& buffer, uint32_t binding) {}
    void DynamicUniforms::UpdateCombinedImageSampler(uint32_t binding) {}
    void DynamicUniforms::Invalidate() {}
}
