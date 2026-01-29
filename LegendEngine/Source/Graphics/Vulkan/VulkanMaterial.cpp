#include <LegendEngine/Graphics/Vulkan/VulkanMaterial.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanTexture2D.hpp>

namespace LegendEngine::Graphics::Vulkan
{
    using namespace Tether::Rendering::Vulkan;

    VulkanMaterial::VulkanMaterial(GraphicsContext& context,
        const VkDescriptorSetLayout layout)
        :
        m_Context(context),
        m_Pool(CreatePool()),
        m_Set(m_Pool, layout, m_Context.GetFramesInFlight()),
        m_Uniforms(m_Context, sizeof(Uniforms), m_Set, 0)
    {}

    void VulkanMaterial::UpdateMaterial()
    {
        Uniforms uniforms;
        uniforms.color = GetColor();

        for (uint32_t i = 0; i < m_Context.GetFramesInFlight(); i++)
            *static_cast<Uniforms*>(m_Uniforms.GetMappedData(i)) = uniforms;

        m_Set.UpdateSets(&m_Uniforms, 0);
        if (const auto pTexture = static_cast<VulkanTexture2D*>(GetTexture());
            pTexture)
            m_Set.UpdateSets(pTexture, 1);
    }

    VkDescriptorSet VulkanMaterial::GetSetAtIndex(const uint32_t index)
    {
        return *m_Set.GetSetAtIndex(index);
    }

    DescriptorPool VulkanMaterial::CreatePool() const
    {
        const uint32_t framesInFlight = m_Context.GetFramesInFlight();

        VkDescriptorPoolSize uniformsSize{};
        uniformsSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformsSize.descriptorCount = framesInFlight;

        VkDescriptorPoolSize samplersSize{};
        samplersSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplersSize.descriptorCount = framesInFlight;

        VkDescriptorPoolSize sizes[] =
        {
            uniformsSize,
            samplersSize
        };

        return DescriptorPool(
            m_Context,
            std::size(sizes) * framesInFlight,
            std::size(sizes),
            sizes
        );
    }
}
