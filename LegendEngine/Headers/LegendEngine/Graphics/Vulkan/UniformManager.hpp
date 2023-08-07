#pragma once

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Graphics/Vulkan/GraphicsContextVk.hpp>

namespace LegendEngine::Vulkan
{
	class UniformManager
	{
	public:
		UniformManager(TetherVulkan::GraphicsContext& context, uint32_t uniformCount, uint32_t images);
		~UniformManager();
		LEGENDENGINE_NO_COPY(UniformManager);

		VkDescriptorPool* GetPool();
	private:
		VkDescriptorPool pool;
		VkDevice m_Device = nullptr;
	};
}