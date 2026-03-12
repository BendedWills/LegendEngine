#include <LegendEngine/LegendEngine.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>

using namespace LegendEngine;

class Headless : public Application
{
public:
    Headless(Graphics::GraphicsContext& ctx)
        :
        Application(ctx)
    {}
private:
    void OnSetup() override
    {
        m_Placeholder = Objects::Create<Objects::Object>();

        GetGlobalScene().AddObject(*m_Placeholder);
    }

    Scope<Objects::Object> m_Placeholder;
};

#include <LegendEngine/Common/Entrypoint.hpp>
LEGENDENGINE_MAIN
{
    const Scope<Graphics::GraphicsContext> ctx = Graphics::GraphicsContext::Create(
        GraphicsAPI::VULKAN, "Headless");
    Graphics::RenderTarget renderTarget(ctx->CreateHeadlessRenderTargetBridge());
    Application& app = Application::CreateHeadless<Headless>(*ctx);

    Stopwatch sw;
    sw.Set();

    while (sw.GetElapsedMillis() < 5000)
    {

    }

    return 0;
}