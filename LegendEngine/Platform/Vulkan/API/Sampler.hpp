#pragma once

#include <LE/Graphics/API/Sampler.hpp>

#include "GraphicsContext.hpp"

namespace le::vk
{
    class Sampler final : public le::Sampler
    {
    public:
        Sampler(GraphicsContext& context, const Info& info);
        ~Sampler() override;
    private:
        TetherVulkan::GraphicsContext& m_context;

        VkSampler m_sampler = nullptr;
    };
}
