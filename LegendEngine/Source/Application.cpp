#include "LegendEngine/Application.hpp"

#include <LegendEngine/Common/Stopwatch.hpp>
#include <LegendEngine/Graphics/GraphicsContext.hpp>
#include <LegendEngine/Events/RenderEvent.hpp>
#include <LegendEngine/Events/UpdateEvent.hpp>

namespace LegendEngine
{
    Scope<Application> Application::m_Instance = nullptr;

    Application::EventHandler::EventHandler(Application& application)
        : m_Application(application)
    {}

    void Application::EventHandler::OnWindowResize(
        const Tether::Events::WindowResizeEvent event
    ) const
    {
        m_Application.ReceiveResize(event.GetNewWidth(), event.GetNewHeight());
    }

    Application::Application(
        const std::string_view applicationName,
        const bool logging,
        const bool debug,
        const GraphicsAPI api)
        :
        m_EventHandler(*this),
        m_Logger(applicationName, logging),
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
        m_Renderer = m_GraphicsContext->CreateRenderer(*this);

        m_Logger.Log(Logger::Level::INFO, "Application created");
    }

    Application::~Application()
    {
        m_Logger.Log(Logger::Level::INFO, "Destroying application");
        m_Window->SetVisible(false);
    }

    void Application::SetupApplication()
    {
        m_IsSetUp = true;

        OnSetup();

        m_Window->SetVisible(true);
        m_Window->AddEventHandler(m_EventHandler, Utils::Events::EventType::WINDOW_RESIZE);

        m_Logger.Log(Logger::Level::INFO, "Application setup complete");
    }

    void Application::SetActiveCamera(Objects::Camera* pCamera)
    {
        if (pCamera)
        {
            const float aspect =
                static_cast<float>(m_Window->GetWidth()) /
                    static_cast<float>(m_Window->GetHeight());
            pCamera->SetAspectRatio(aspect);
        }

        m_pActiveCamera = pCamera;
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
        return *m_GraphicsContext;
    }

    Events::EventBus& Application::GetEventBus()
    {
        return m_EventBus;
    }

    Scene& Application::GetGlobalScene()
    {
        return m_GlobalScene;
    }

    Tether::Window& Application::GetWindow() const
    {
        return *m_Window;
    }

    Graphics::Renderer& Application::GetRenderer() const
    {
        return *m_Renderer;
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

    bool Application::IsCloseRequested() const
    {
        return m_Window->IsCloseRequested();
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
        while (!IsCloseRequested())
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
        if (updateWindow)
            Tether::Application::Get().PollEvents();

        RecalculateTransforms(m_GlobalScene);
        if (m_pActiveScene)
            RecalculateTransforms(*m_pActiveScene);

        OnUpdate(delta);
        m_EventBus.DispatchEvent<Events::UpdateEvent>(Events::UpdateEvent(delta));
    }

    void Application::Render(const float delta)
    {
        if (!m_Renderer)
            return;

        m_Renderer->RenderFrame();

        OnRender(delta);
        m_EventBus.DispatchEvent<Events::RenderEvent>(Events::RenderEvent(delta));
    }

    void Application::RecalculateTransforms(Scene& scene)
    {
        for (Objects::Object* object : scene.GetObjects())
            if (object->IsDirty())
                object->CalculateTransformMatrix();
    }

    void Application::ReceiveResize(const uint64_t width, const uint64_t height)
    {
        m_Renderer->NotifyWindowResized();

        const float aspect = static_cast<float>(width) / static_cast<float>(height);
        if (m_pActiveCamera)
            m_pActiveCamera->SetAspectRatio(aspect);

        OnResize(width, height);
    }
}
