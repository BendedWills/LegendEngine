#include <LegendEngine/Common/Defs.hpp>

#include <LegendEngine/Graphics/Vulkan/UniformManager.hpp>

namespace LegendEngine::Vulkan
{
	UniformManager::UniformManager(TetherVulkan::GraphicsContext& context, uint32_t uniformCount, uint32_t images)
		:
		m_Device(context.GetDevice())
	{
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = uniformCount * images;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = poolSize.descriptorCount;

		if (vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &pool)
			!= VK_SUCCESS)
			throw std::runtime_error("Failed to create descriptor pool!");
	}

	UniformManager::~UniformManager()
	{
#if !defined(NDEBUG)
		if (!pRegisteredUniforms.empty())
		{
			std::cerr << "All UniformBuffer objects must be destroyed before the UniformManager!\n";
		}
#endif

		vkDeviceWaitIdle(m_Device);
		vkDestroyDescriptorPool(m_Device, pool, nullptr);
	}

	void UniformManager::RegisterUniformBuffer(UniformBuffer* uniform)
	{
		if (std::find(pRegisteredUniforms.begin(), pRegisteredUniforms.end(), uniform)
			!= pRegisteredUniforms.end())
			return;

		pRegisteredUniforms.push_back(uniform);
	}

	void UniformManager::UnregisterUniformBuffer(UniformBuffer* uniform)
	{
		for (size_t i = 0; i < pRegisteredUniforms.size(); i++)
			if (pRegisteredUniforms[i] == uniform)
				pRegisteredUniforms.erase(pRegisteredUniforms.begin() + i);
	}

	VkDescriptorPool* UniformManager::GetPool()
	{
		return &pool;
	}
}
