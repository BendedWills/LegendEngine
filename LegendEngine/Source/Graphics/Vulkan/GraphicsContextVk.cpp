#include <LegendEngine/Graphics/Vulkan/GraphicsContextVk.hpp>

#include <vulkan/vulkan.h>

namespace LegendEngine::Vulkan
{
	GraphicsContext::DebugCallback::DebugCallback(GraphicsContext& context)
		:
		m_Context(context)
	{}

	void GraphicsContext::DebugCallback::OnDebugLog(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
	)
	{
		std::stringstream ss;
		ss << "Vulkan Validation Layer: " << pCallbackData->pMessage;

		std::cout << ss.str() << '\n';
	}

	GraphicsContext::GraphicsContext(std::string_view applicationName, bool debug)
		:
		m_ContextCreator(debug, applicationName, "LegendEngine"),
		m_GraphicsContext(m_ContextCreator),
		m_Callback(*this)
	{
		m_ContextCreator.AddDebugMessenger(&m_Callback);
	}

	GraphicsContext::~GraphicsContext()
	{
		m_ContextCreator.RemoveDebugMessenger(&m_Callback);
	}

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