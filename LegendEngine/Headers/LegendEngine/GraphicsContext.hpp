#ifndef _LEGENDENGINE_CONTEXT_HPP
#define _LEGENDENGINE_CONTEXT_HPP

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Common/IDisposable.hpp>

#include <LegendEngine/Graphics/Vulkan/Instance.hpp>

namespace LegendEngine
{
	class GraphicsContext
	{
	public:
		static GraphicsContext& Create(RenderingAPI api, bool debug);
		static GraphicsContext& Get();
	private:
		inline static Scope<GraphicsContext> internal = nullptr;
	};
}

#endif //_LEGENDENGINE_CONTEXT_HPP