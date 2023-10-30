#include <LegendEngine/Graphics/Vulkan/ShaderModule.hpp>

namespace LegendEngine::Vulkan
{
	ShaderModule::ShaderModule(TetherVulkan::GraphicsContext& context, ShaderType type,
		uint32_t* spv, uint64_t spvSize)
		:
		m_Device(context.GetDevice()),
		type(type)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = spvSize;
		createInfo.pCode = spv;

		if (vkCreateShaderModule(m_Device, &createInfo, nullptr,
			&shaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed to create shader module");
	}

	ShaderModule::~ShaderModule()
	{
		vkDestroyShaderModule(m_Device, shaderModule, nullptr);
	}

	LegendEngine::ShaderType ShaderModule::GetType()
	{
		return type;
	}

	VkShaderModule ShaderModule::Get()
	{
		return shaderModule;
	}
}