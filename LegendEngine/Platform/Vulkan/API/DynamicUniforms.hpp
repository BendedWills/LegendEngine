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
        void UpdateSampledImage(le::Image& image, uint32_t binding) override;
        void UpdateSampler(le::Sampler& sampler, uint32_t binding) override;
        void UpdateCombinedImageSampler(le::Image& image, le::Sampler& sampler, uint32_t binding) override;

        void InvalidateBinding(uint32_t binding) override;

        static VkDescriptorType GetDescriptorType(DescriptorType type);
    private:
        struct Binding
        {
            size_t framesUntilValid = 0;
            std::vector<VkDescriptorSet> descriptorSets;
            VkDescriptorType descriptorType;
        };

        void CreateDescriptorPool(std::span<DescriptorInfo> infos);
        VkDescriptorSet GetDescriptorSet(uint32_t binding);
        void ValidateSet(uint32_t binding);
        void WriteSet(uint32_t binding, VkWriteDescriptorSet* pWrite);

        static size_t GetDescriptorSetCount(const DescriptorInfo& info);

        TetherVulkan::GraphicsContext& m_context;

        VkDescriptorPool m_descriptorPool = nullptr;

        std::vector<Binding> m_bindings;
    };
}
