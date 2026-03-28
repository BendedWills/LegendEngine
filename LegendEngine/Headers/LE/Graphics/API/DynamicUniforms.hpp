#pragma once

#include <cstdint>
#include <LE/Graphics/API/Buffer.hpp>
#include <LE/Graphics/API/DescriptorSetLayout.hpp>
#include <LE/Graphics/API/Image.hpp>
#include <LE/Graphics/API/Sampler.hpp>
#include <LE/Graphics/API/Types.hpp>

namespace le
{
    class DynamicUniforms
    {
    public:
        enum class UpdateFrequency
        {
            OCCASIONAL,
            PER_FRAME,
        };

        struct DescriptorInfo
        {
            DescriptorType type = DescriptorType::UNIFORM_BUFFER;
            UpdateFrequency updateFrequency = UpdateFrequency::OCCASIONAL;
            size_t amount = 0;
            DescriptorSetLayout* pLayout = nullptr;
        };

        virtual ~DynamicUniforms() = default;

        virtual void UpdateBuffer(Buffer& buffer, uint32_t binding) = 0;
        virtual void UpdateSampledImage(Image& image, uint32_t binding) = 0;
        virtual void UpdateSampler(Sampler& sampler, uint32_t binding) = 0;
        virtual void UpdateCombinedImageSampler(Image& image, Sampler& sampler, uint32_t binding) = 0;

        virtual void InvalidateBinding(uint32_t binding) = 0;
    };
}
