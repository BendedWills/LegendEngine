#pragma once

#include <LE/Graphics/API/Types.hpp>

namespace le
{
    class DescriptorSetLayout
    {
    public:
        struct Binding
        {
            size_t binding = 0;
            size_t descriptorCount = 0;
            DescriptorType descriptorType = DescriptorType::UNIFORM_BUFFER;
            ShaderStageFlags stageFlags = ShaderStageFlags::ALL;
        };

        virtual ~DescriptorSetLayout() = default;
    };
}
