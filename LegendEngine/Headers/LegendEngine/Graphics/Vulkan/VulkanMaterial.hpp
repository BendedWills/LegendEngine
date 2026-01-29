#pragma once

#include <LegendEngine/Resources/Material.hpp>
#include <Tether/Rendering/Vulkan/DescriptorSet.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>
#include <Tether/Rendering/Vulkan/UniformBuffer.hpp>

#include <vulkan/vulkan.h>

namespace LegendEngine::Graphics::Vulkan
{
    using namespace Tether::Rendering::Vulkan;

    class VulkanMaterial final : public Resources::Material
    {
    public:
        struct Uniforms
        {
            Color color = Color(1.0f);
        };

        VulkanMaterial(GraphicsContext& context,
            VkDescriptorSetLayout layout);

        void UpdateIfChanged() override;

        VkDescriptorSet GetSetAtIndex(uint32_t index);
    private:
        DescriptorPool CreatePool() const;

        GraphicsContext& m_Context;

        DescriptorPool m_Pool;
        DescriptorSet m_Set;
        UniformBuffer m_Uniforms;
    };
}
