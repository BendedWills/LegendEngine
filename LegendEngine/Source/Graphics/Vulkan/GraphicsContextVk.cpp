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

		if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			pApplication->Log(ss.str(), LogType::ERROR);
		else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			pApplication->Log(ss.str(), LogType::WARN);
	}

	GraphicsContext::GraphicsContext(std::string_view applicationName, bool debug)
		:
		m_ContextCreator(debug, applicationName, "LegendEngine"),
		m_GraphicsContext(m_ContextCreator)
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