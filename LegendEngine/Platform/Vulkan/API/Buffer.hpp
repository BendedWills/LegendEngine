#pragma once

#include <LE/Graphics/API/Buffer.hpp>
#include <vulkan/vulkan.h>

namespace le::vk
{
    class Buffer : public le::Buffer
    {
    public:
        virtual VkBuffer GetBuffer() = 0;
    };
}
