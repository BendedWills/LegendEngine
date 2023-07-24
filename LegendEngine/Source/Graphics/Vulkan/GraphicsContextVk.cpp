#include <LegendEngine/Graphics/Vulkan/GraphicsContextVk.hpp>

#include <vulkan/vulkan.h>

namespace LegendEngine::Vulkan
{
	GraphicsContext::GraphicsContext(std::string_view applicationName, bool debug)
		:
		m_ContextCreator(debug, applicationName, "LegendEngine"),
		m_GraphicsContext(m_ContextCreator)
	{}

	TetherVulkan::InstanceInfo GraphicsContext::CreateInstanceInfo(
		const char* appName)
	{
		TetherVulkan::InstanceInfo info;
		info.applicationName = appName;
		info.engineName = "LegendEngine";
		info.CreateInstance = vkCreateInstance;
		info.EnumerateInstanceExtensionProperties = vkEnumerateInstanceExtensionProperties;
		info.EnumerateInstanceLayerProperties = vkEnumerateInstanceLayerProperties;
		info.GetInstanceProcAddr = vkGetInstanceProcAddr;

		return info;
	}

	TetherVulkan::GraphicsContext& GraphicsContext::GetTetherGraphicsContext()
	{
		return m_GraphicsContext;
	}
}