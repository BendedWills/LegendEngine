#include "LegendEngine/Application.hpp"

#include <LegendEngine/Common/Stopwatch.hpp>
#include <LegendEngine/Graphics/GraphicsContext.hpp>
#include <LegendEngine/Events/RenderEvent.hpp>
#include <LegendEngine/Events/UpdateEvent.hpp>

#include "LegendEngine/Graphics/WindowRenderTarget.hpp"

namespace LegendEngine
{
    Scope<Application> Application::m_Instance = nullptr;

#ifndef LE_HEADLESS
    Application::Application(
        int width, int height,
        std::string_view applicationName,
        GraphicsAPI api)
        :
        m_GraphicsContext(CreateGraphicsContext(applicationName, api)),
        m_RenderTarget(CreateRenderTarget(width, height, applicationName)),
        m_Renderer(CreateRenderer()),
        m_GlobalScene(m_EventBus)
    {
        LE_INFO("Application created");
    }

    Graphics::WindowRenderTarget& Application::GetWindowRenderTarget() const
    {
        return *m_WindowRenderTarget;
    }

    Graphics::Renderer& Application::GetRenderer() const
    {
        return m_Renderer;
    }

    void Application::SetActiveCamera(Objects::Camera* pCamera)
    {
        m_pActiveCamera = pCamera;
        m_RenderTarget.SetCamera(pCamera);
    }

    Graphics::RenderTarget& Application::GetRenderTarget() const
    {
        return m_RenderTarget;
    }

    void Application::Render(const float delta)
    {
        Scene* scenes[] = { &m_GlobalScene, m_pActiveScene };
        m_Renderer.RenderFrame(scenes);

        OnRender(delta);
        m_EventBus.DispatchEvent<Events::RenderEvent>(Events::RenderEvent(delta));
    }

    void Application::Run()
    {
        Stopwatch deltaTimer;
        while (!m_RenderTarget.IsCloseRequested())
        {
            const float delta = deltaTimer.GetElapsedMillis() / 1000.0f;
            deltaTimer.Set();

            RenderFrame(delta);
        }
    }

    void Application::RenderFrame(const float delta)
    {
        Update(delta);
        Render(delta);
    }

    Graphics::GraphicsContext& Application::CreateGraphicsContext(std::string_view applicationName,
                                                                  GraphicsAPI api)
    {
        LE_INFO("Creating graphics context");
        return *(m_ManagedGraphicsContext = Graphics::GraphicsContext::Create(api, applicationName));
    }

    Graphics::RenderTarget& Application::CreateRenderTarget(int width, int height,
                                                            std::string_view applicationName)
    {
        std::wstring title(applicationName.size(), L' ');
        std::mbstowcs(title.data(), applicationName.data(),
                      applicationName.size());

        LE_INFO("Creating window");
        m_Window = Tether::Window::Create(width, height,
                                          title, false);

        LE_INFO("Creating render target");
        return *(m_WindowRenderTarget = std::make_unique<Graphics::WindowRenderTarget>(*m_ManagedGraphicsContext, *m_Window));
    }

    Graphics::Renderer& Application::CreateRenderer()
    {
        LE_INFO("Creating renderer");
        return *(m_ManagedRenderer = m_ManagedGraphicsContext->CreateRenderer(m_RenderTarget));
    }
#else
    Application::Application(Graphics::GraphicsContext& ctx)
        :
        m_GraphicsContext(ctx),
        m_GlobalScene(m_EventBus)
    {
        LE_INFO("Application created");
    }
#endif

    Application::~Application()
    {
        LE_INFO("Destroying application");

#ifndef LE_HEADLESS
        m_RenderTarget.SetVisible(false);
#endif
    }

    void Application::SetActiveScene(Scene& scene)
    {
        m_pActiveScene = &scene;
    }

    void Application::ClearActiveScene()
    {
        m_pActiveScene = nullptr;
    }

    Graphics::GraphicsContext& Application::GetGraphicsContext() const
    {
        return m_GraphicsContext;
    }

    Events::EventBus& Application::GetEventBus()
    {
        return m_EventBus;
    }

    Scene& Application::GetGlobalScene()
    {
        return m_GlobalScene;
    }

    Objects::Camera* Application::GetActiveCamera() const
    {
        return m_pActiveCamera;
    }

    Scene* Application::GetActiveScene() const
    {
        return m_pActiveScene;
    }

    void Application::Destroy()
    {
        m_Instance.reset();
    }

    bool Application::HasConstructed()
    {
        return m_Instance != nullptr;
    }

    Application& Application::Get()
    {
        if (!m_Instance)
            throw std::runtime_error(
                "Application::Get() called before it was constructed. "
                "No LegendEngine objects may be used before Application::Run(), "
                "Application::Destroy must be called if running in headless mode");

        if (!m_Instance->m_IsSetUp)
            throw std::runtime_error(
                "Application not set up"
                "No LegendEngine objects may be used before SetupApplication()");

        return *m_Instance;
    }

    void Application::SetupApplication()
    {
        m_IsSetUp = true;

        OnSetup();

#ifndef LE_HEADLESS
        m_RenderTarget.SetVisible(true);
#endif

        LE_INFO("Application setup complete");
    }

    void Application::Update(const float delta, const bool updateWindow)
    {
#ifndef LE_HEADLESS
        if (updateWindow)
            Tether::Application::Get().PollEvents();
#endif

        RecalculateTransforms(m_GlobalScene);
        if (m_pActiveScene)
            RecalculateTransforms(*m_pActiveScene);

        OnUpdate(delta);
        m_EventBus.DispatchEvent<Events::UpdateEvent>(Events::UpdateEvent(delta));
    }

    void Application::RecalculateTransforms(Scene& scene)
    {
        for (Objects::Object* object : scene.GetObjects())
            if (object->IsDirty())
                object->CalculateTransformMatrix();
    }
}
