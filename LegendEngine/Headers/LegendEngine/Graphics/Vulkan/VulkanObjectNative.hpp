#ifndef _LEGENDENGINE_VULKAN_VULKANOBJECTNATIVE_HPP
#define _LEGENDENGINE_VULKAN_VULKANOBJECTNATIVE_HPP
#ifdef VULKAN_API

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Objects/Object.hpp>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace LegendEngine::Vulkan
{
	class VulkanRenderer;
	class VulkanObjectNative : public Objects::IObjectNative
	{
	public:
		LEGENDENGINE_NO_COPY(VulkanObjectNative);

		VulkanObjectNative(Objects::Object* pObject, VulkanRenderer* pRenderer);

		void SetCurrentImage(uint64_t imageIndex);
		void OnUniformsUpdate();

		VkDescriptorSet* GetDescriptorSets();

		void OnDispose();
	private:
		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<VmaAllocation> allocations;
		std::vector<VkBuffer> uniformBuffers;

		VkDescriptorPool descriptorPool;
		
		uint64_t images = 0;
		uint64_t imageIndex = 0;

		VulkanRenderer* pRenderer = nullptr;
	};
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_VULKANOBJECTNATIVE_HPP