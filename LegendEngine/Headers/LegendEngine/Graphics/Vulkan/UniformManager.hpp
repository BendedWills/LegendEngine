#ifndef _LEGENDENGINE_VULKAN_UNIFORMMANAGER_HPP
#define _LEGENDENGINE_VULKAN_UNIFORMMANAGER_HPP

#include <LegendEngine/Common/Defs.hpp>
#ifdef VULKAN_API

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Graphics/Vulkan/Device.hpp>

namespace LegendEngine::Vulkan
{
	class UniformManager : public IDisposable
	{
	public:
		UniformManager() {}
		LEGENDENGINE_NO_COPY(UniformManager);

		bool Init(Device* pDevice, uint32_t uniformCount, uint32_t images);

		VkDescriptorPool* GetPool();
	private:
		void OnDispose();

		VkDescriptorPool pool;
		Device* pDevice = nullptr;
	};
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_UNIFORMBUFFER_HPP
