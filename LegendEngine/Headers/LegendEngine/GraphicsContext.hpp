#pragma once

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/Ref.hpp>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Common/IDisposable.hpp>

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
