#include <LegendEngine/LegendEngine.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>

using namespace LegendEngine;

class Headless : public Application
{
public:
    Headless(Graphics::GraphicsContext& ctx, Graphics::Renderer& renderer)
        :
        Application("Headless", true, true, ctx, renderer)
    {}
private:
    void OnSetup() override
    {
        m_Placeholder = Objects::Create<Objects::Object>();

        GetGlobalScene().AddObject(*m_Placeholder);
    }

    Scope<Objects::Object> m_Placeholder;
};

#include <LegendEngine/Common/Platform.hpp>
LEGENDENGINE_MAIN
{
    const Scope<Graphics::GraphicsContext> ctx = Graphics::GraphicsContext::Create(
        GraphicsAPI::VULKAN, "Headless", true);
    Graphics::RenderTarget renderTarget(ctx->CreateHeadlessRenderTargetBridge());
    const Scope<Graphics::Renderer> renderer = ctx->CreateRenderer(renderTarget);
    Application& app = Application::CreateHeadless<Headless>(*ctx, *renderer);

    Stopwatch sw;
    sw.Set();

    while (sw.GetElapsedMillis() < 5000)
    {

    }

    return 0;
}