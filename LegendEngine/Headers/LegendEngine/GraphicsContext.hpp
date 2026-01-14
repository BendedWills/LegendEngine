#pragma once

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/Ref.hpp>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Graphics/IRenderer.hpp>

#include <Tether/Window.hpp>

namespace LegendEngine
{
	class GraphicsContext
	{
		friend class Application;
	public:
		virtual ~GraphicsContext() = default;

		static GraphicsContext& Create(RenderingAPI api, bool debug);
		static GraphicsContext& Get();
	protected:
		virtual Scope<IRenderer> CreateRenderer(Application& app, Tether::Window& window) = 0;
	private:
		inline static Scope<GraphicsContext> internal = nullptr;
	};
}
