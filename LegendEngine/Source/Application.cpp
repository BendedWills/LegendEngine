#include "LegendEngine/Application.hpp"

#include <LegendEngine/Common/Stopwatch.hpp>
#include <LegendEngine/Graphics/GraphicsContext.hpp>
#include <LegendEngine/Events/RenderEvent.hpp>
#include <LegendEngine/Events/UpdateEvent.hpp>

namespace LegendEngine
{
    Scope<Application> Application::m_Instance = nullptr;

#ifndef LGENG_HEADLESS
    Application::Application(
        bool logging,
        bool debug,
        Graphics::GraphicsContext& gfxContext,
        Graphics::RenderTarget& renderTarget)
        :
        m_EventHandler(*this),
        m_Logger(applicationName, logging, debug),
        m_GlobalScene(m_EventBus),
        m_Debug(debug)
    {
        std::wstring title(applicationName.size(), L' ');
        std::mbstowcs(title.data(), applicationName.data(),
            applicationName.size());

        m_Logger.Log(Logger::Level::INFO, "Creating window");
        m_Window = Tether::Window::Create(1280, 720,
            title, false);
        m_GraphicsContext = Graphics::GraphicsContext::Create(api, applicationName,
            debug, m_Logger);
        m_Logger.Log(Logger::Level::INFO, "Creating renderer");
        m_Renderer = m_GraphicsContext->CreateRenderer(*this);
        m_Logger.Log(Logger::Level::INFO, "Renderer created");

        m_Logger.Log(Logger::Level::INFO, "Application created");
    }
#endif

    Application::Application(const std::string_view applicationName,
        const bool logging, const bool debug,
        Graphics::GraphicsContext& graphicsContext,
        Graphics::Renderer& renderer)
        :
        m_GraphicsContext(graphicsContext),
        m_RenderTarget(renderer.GetRenderTarget()),
        m_Renderer(renderer),
        m_Logger(applicationName, logging, debug),
        m_GlobalScene(m_EventBus),
        m_Debug(debug)
    {
        m_Logger.Log(Logger::Level::INFO, "Application created");
    }

    Application::~Application()
    {
        m_Logger.Log(Logger::Level::INFO, "Destroying application");
        m_RenderTarget.SetVisible(false);
    }

    void Application::SetupApplication()
    {
        m_IsSetUp = true;

        OnSetup();

        m_RenderTarget.SetVisible(true);

        m_Logger.Log(Logger::Level::INFO, "Application setup complete");
    }

    void Application::SetActiveCamera(Objects::Camera* pCamera)
    {
        m_pActiveCamera = pCamera;
        m_RenderTarget.SetCamera(pCamera);
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

    Graphics::RenderTarget& Application::GetRenderTarget() const
    {
        return m_RenderTarget;
    }

    Events::EventBus& Application::GetEventBus()
    {
        return m_EventBus;
    }

    Scene& Application::GetGlobalScene()
    {
        return m_GlobalScene;
    }

    Graphics::Renderer& Application::GetRenderer() const
    {
        return m_Renderer;
    }

    Objects::Camera* Application::GetActiveCamera() const
    {
        return m_pActiveCamera;
    }

    Scene* Application::GetActiveScene() const
    {
        return m_pActiveScene;
    }

    Logger& Application::GetLogger()
    {
        return m_Logger;
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
                "No LegendEngine objects may be used before Application::Run()");

        if (!m_Instance->m_IsSetUp)
            throw std::runtime_error(
                "Application not set up"
                "No LegendEngine objects may be used before SetupApplication()");

        return *m_Instance;
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

    void Application::Update(const float delta, const bool updateWindow)
    {
#ifndef LGENG_HEADLESS
        if (updateWindow)
            Tether::Application::Get().PollEvents();
#endif

        RecalculateTransforms(m_GlobalScene);
        if (m_pActiveScene)
            RecalculateTransforms(*m_pActiveScene);

        OnUpdate(delta);
        m_EventBus.DispatchEvent<Events::UpdateEvent>(Events::UpdateEvent(delta));
    }

    void Application::Render(const float delta)
    {
        Scene* scenes[] = { &m_GlobalScene, m_pActiveScene };
        m_Renderer.RenderFrame(scenes);

        OnRender(delta);
        m_EventBus.DispatchEvent<Events::RenderEvent>(Events::RenderEvent(delta));
    }

    void Application::RecalculateTransforms(Scene& scene)
    {
        for (Objects::Object* object : scene.GetObjects())
            if (object->IsDirty())
                object->CalculateTransformMatrix();
    }
}
