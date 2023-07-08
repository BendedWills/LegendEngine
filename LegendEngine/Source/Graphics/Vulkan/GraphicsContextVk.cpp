#include <LegendEngine/Graphics/Vulkan/GraphicsContextVk.hpp>

#include <vulkan/vulkan.h>

namespace LegendEngine::Vulkan
{
	GraphicsContext::GraphicsContext(std::string_view applicationName, bool debug)
		:
		m_Instance(CreateInstanceInfo(applicationName.data()), debug)
	{}

	Instance& GraphicsContext::GetInstance() const
	{
		return m_Instance;
	}

	TetherVulkan::InstanceInfo GraphicsContext::CreateInstanceInfo(
		const char* appName)
	{
		TetherVulkan::InstanceInfo info;
		info.applicationName = appName.data();
		info.engineName = "LegendEngine";
		info.CreateInstance = vkCreateInstance;
		info.EnumerateInstanceExtensionProperties = vkEnumerateInstanceExtensionProperties;
		info.EnumerateInstanceLayerProperties = vkEnumerateInstanceLayerProperties;
		info.GetInstanceProcAddr = vkGetInstanceProcAddr;

		return info;
	}
}