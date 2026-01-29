#pragma once

#include <LegendEngine/Resources/Material.hpp>
#include <Tether/Rendering/Vulkan/DescriptorSet.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>
#include <Tether/Rendering/Vulkan/UniformBuffer.hpp>

#include <vulkan/vulkan.h>

namespace LegendEngine::Graphics::Vulkan
{
    class VulkanMaterial final : public Resources::Material
    {
    public:
        struct Uniforms
        {
            Color color = Color(1.0f);
        };

        VulkanMaterial(Tether::Rendering::Vulkan::GraphicsContext& context,
            VkDescriptorSetLayout layout);

        void UpdateMaterial() override;

        VkDescriptorSet GetSetAtIndex(uint32_t index);
    private:
        Tether::Rendering::Vulkan::DescriptorPool CreatePool() const;

        Tether::Rendering::Vulkan::GraphicsContext& m_Context;

        Tether::Rendering::Vulkan::DescriptorPool m_Pool;
        Tether::Rendering::Vulkan::DescriptorSet m_Set;
        Tether::Rendering::Vulkan::UniformBuffer m_Uniforms;
    };
}
