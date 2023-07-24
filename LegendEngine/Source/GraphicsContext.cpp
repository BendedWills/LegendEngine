#include <LegendEngine/GraphicsContext.hpp>

#ifdef VULKAN_API
#include <LegendEngine/Graphics/Vulkan/GraphicsContextVk.hpp>
#endif

#ifdef OPENGL_API
// Include something
#endif

namespace LegendEngine
{
	GraphicsContext& GraphicsContext::Create(RenderingAPI api, bool debug)
	{
		switch (api)
		{
#ifdef VULKAN_API
			case RenderingAPI::VULKAN:
			{
				internal = std::make_unique<Vulkan::GraphicsContext>("LegendEngineApp",
					debug);
			}
			break;
#endif

#ifdef OPENGL_API
			
#endif
			
			default:
			{
				throw std::runtime_error(
					"LegendEngine wasn't compiled with any graphics library enabled"
				);
			}
			break;
		}

		return *internal;
	}

	GraphicsContext& GraphicsContext::Get()
	{
		if (!internal.get())
			throw std::runtime_error(
				"Graphics context is uninitialized! "
				"You must call GraphicsContext::Create at the start of " 
				"the application"
			);

		return *internal;
	}
}
