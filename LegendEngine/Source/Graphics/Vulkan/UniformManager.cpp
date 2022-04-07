#include <LegendEngine/Common/Defs.hpp>
#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/UniformManager.hpp>

using namespace LegendEngine::Vulkan;

bool UniformManager::Init(Device* pDevice, uint32_t uniformCount, uint32_t images)
{
    if (initialized)
        return false;
    this->pDevice = pDevice;

	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = uniformCount * images;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = poolSize.descriptorCount;

    if (vkCreateDescriptorPool(pDevice->Get(), &poolInfo, nullptr, &pool)
        != VK_SUCCESS)
        return false;

    initialized = true;
    return true;
}

VkDescriptorPool* UniformManager::GetPool()
{
    return &pool;
}

void UniformManager::OnDispose()
{
	vkDestroyDescriptorPool(pDevice->Get(), pool, nullptr);
}

#endif // VULKAN_API