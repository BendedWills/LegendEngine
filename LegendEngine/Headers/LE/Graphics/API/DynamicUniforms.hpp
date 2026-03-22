#pragma once

#include <LE/Graphics/API/Buffer.hpp>
#include <cstdint>

namespace le
{
    class DynamicUniforms
    {
    public:
        enum class DescriptorType
        {
            UNIFORM_BUFFER,
            COMBINED_IMAGE_SAMPLER
        };

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
        };

        virtual ~DynamicUniforms() = default;

        virtual void UpdateBuffer(Buffer& buffer, uint32_t binding) = 0;
        virtual void UpdateCombinedImageSampler(uint32_t binding) = 0;

        uint32_t m_currentFrame = 0;
    };
}
