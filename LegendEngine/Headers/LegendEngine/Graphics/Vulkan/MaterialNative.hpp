#pragma once

#include <LegendEngine/Common/Defs.hpp>

#include <LegendEngine/Graphics/Vulkan/UniformBuffer.hpp>
#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Resources/Material.hpp>
#include <LegendEngine/Graphics/Vulkan/GraphicsContextVk.hpp>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace LegendEngine::Vulkan
{
    class VulkanRenderer;
    class MaterialNative : public Resources::IMaterialNative
    {
        friend VulkanRenderer;
    public:
        LEGENDENGINE_NO_COPY(MaterialNative);
        
        MaterialNative(TetherVulkan::GraphicsContext& context,
            uint32_t images, VkDescriptorSetLayout layout, 
            Resources::Material* pMaterial);
    protected:
        std::optional<Vulkan::UniformBuffer> uniform;
        VkDescriptorPool pool;

        uint32_t images = 0;
        
        TetherVulkan::GraphicsContext& m_GraphicsContext;
        VkDevice m_Device = nullptr;

        VkDescriptorSetLayout m_Layout = nullptr;
    private:
		bool OnCreate();
		void OnUpdate();
		void OnUpdateUniforms();
		void OnDispose();

        void UpdateDescriptorSets();
    };
}
