#pragma once

#include <LE/Graphics/API/DescriptorSetLayout.hpp>

#include "GraphicsContext.hpp"

namespace le::vk
{
    class DescriptorSetLayout : public le::DescriptorSetLayout
    {
    public:
        explicit DescriptorSetLayout(GraphicsContext& context);
        ~DescriptorSetLayout() override;

        VkDescriptorSetLayout GetDescriptorSetLayout() const;
    private:
        VkDescriptorSetLayout m_descriptorSetLayout = nullptr;
    };
}
