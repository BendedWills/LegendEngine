#ifndef _LEGENDENGINE_VULKAN_VULKANOBJECTNATIVE_HPP
#define _LEGENDENGINE_VULKAN_VULKANOBJECTNATIVE_HPP

#include <LegendEngine/Common/Defs.hpp>
#ifdef VULKAN_API

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Objects/Object.hpp>

#include <LegendEngine/Graphics/Vulkan/UniformBuffer.hpp>
#include <LegendEngine/Graphics/Vulkan/UniformManager.hpp>

namespace LegendEngine::Vulkan
{
	class VulkanRenderer;
	class ObjectNative : public Objects::IObjectNative
	{
	public:
		LEGENDENGINE_NO_COPY(ObjectNative);
		ObjectNative(TetherVulkan::GraphicsContext& context, 
			VkDescriptorSetLayout objectSetLayout,
			Objects::Object* pObject);

		void SetCurrentImage(uint64_t imageIndex);
		void OnUniformsUpdate();

		Vulkan::UniformBuffer* GetUniform();
	private:
		uint32_t m_Images = 0;
		uint64_t imageIndex = 0;

		Vulkan::UniformBuffer m_Uniform;
		Vulkan::UniformManager m_UniformManager;
		IObjectNative::ObjectUniforms ubo;
	};
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_VULKANOBJECTNATIVE_HPP