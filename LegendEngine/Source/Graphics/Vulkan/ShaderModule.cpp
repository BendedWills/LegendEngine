#include <LegendEngine/Graphics/Vulkan/ShaderModule.hpp>

namespace LegendEngine::Vulkan
{
	ShaderModule::ShaderModule(const TetherVulkan::GraphicsContext& context,
		const Resources::ShaderStage& stage)
		:
		m_Device(context.GetDevice()),
		type(stage.type)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = stage.compiledShaderSize;
		createInfo.pCode = reinterpret_cast<const uint32_t*>(
			stage.compiledShaderCode);

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