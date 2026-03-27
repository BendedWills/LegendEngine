#pragma once

#include <LE/Graphics/API/DynamicUniforms.hpp>

namespace le::vk
{
    class DynamicUniforms final : public le::DynamicUniforms
    {
    public:
        void UpdateBuffer(le::Buffer& buffer, uint32_t binding) override;

        void UpdateCombinedImageSampler(uint32_t binding) override;


    private:
    };
}
