#include <LegendEngine/Context.hpp>

using namespace LegendEngine;

bool ContextInternal::InitAPI(RenderingAPI api, bool debugMode)
{
	switch (api)
	{
	#ifdef VULKAN_API
		case RenderingAPI::VULKAN:
		{
			 return InitVulkan(debugMode);
		}
		break;
	#endif
#if 0
	#ifdef OPENGL_API
		case RenderingAPI::OPENGL:
		{
			return InitOpenGL();
		}
		break;
	#endif
#endif
	}

	return false;
}

#ifdef VULKAN_API
bool ContextInternal::InitVulkan(bool debugMode)
{
	if (vulkanInitialized)
		return true;

	if (!instance.Init("LegendEngineApp", "LegendEngine", debugMode))
		return false;

	vulkanInitialized = true;
	return true;
}
#endif

void ContextInternal::OnDispose()
{
#ifdef VULKAN_API
	if (vulkanInitialized)
	{
		instance.Dispose();
	} vulkanInitialized = false;
#endif // VULKAN_API
}

bool Context::InitAPI(RenderingAPI api, bool debugMode)
{
	return context.InitAPI(api, debugMode);
}

Vulkan::Instance* Context::GetVulkanInstance()
{
	return &context.instance;
}

bool Context::IsVulkanInitialized()
{
	return context.vulkanInitialized;
}

void Context::Dispose()
{
	context.Dispose();
}