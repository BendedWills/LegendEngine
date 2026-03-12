#pragma once

#include <memory>
#include <LegendEngine/Scene.hpp>
#include <LegendEngine/Common/Assert.hpp>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Events/EventBus.hpp>
#include <LegendEngine/Graphics/GraphicsContext.hpp>
#include <LegendEngine/Graphics/Renderer.hpp>
#include <LegendEngine/Graphics/RenderTarget.hpp>
#include <LegendEngine/IO/Logger.hpp>
#include <LegendEngine/Objects/Camera.hpp>
#include <Tether/Window.hpp>

#include "Graphics/WindowRenderTarget.hpp"

#ifndef LGENG_HEADLESS
#include <LegendEngine/Graphics/WindowRenderTarget.hpp>
#endif

namespace LegendEngine
{
    namespace Input = Tether::Input;
    namespace Utils = Tether;

    class Application
    {
    public:
#ifndef LGENG_HEADLESS
        // Creates the Application with a WindowRenderTarget
        Application(
            int width, int height,
            std::string_view applicationName,
            GraphicsAPI api);

        Graphics::WindowRenderTarget& GetWindowRenderTarget() const;
        [[nodiscard]] Graphics::Renderer& GetRenderer() const;
#else
        Application(Graphics::GraphicsContext& ctx); // Headless application constructor
#endif
        virtual ~Application() = 0;

        LEGENDENGINE_NO_COPY(Application);

        void SetActiveCamera(Objects::Camera* pCamera);
        void SetActiveScene(Scene& scene);

        void ClearActiveScene();

        Graphics::GraphicsContext& GetGraphicsContext() const;
        Graphics::RenderTarget& GetRenderTarget() const;
        Events::EventBus& GetEventBus();
        Scene& GetGlobalScene();
        Objects::Camera* GetActiveCamera() const;
        Scene* GetActiveScene() const;

        void Update(float delta, bool updateWindow = true);
        void Render(float delta);

        template<typename T, typename... Args>
            requires std::is_base_of_v<Application, T>
        static int RunApplication(Args&&... args)
        {
            LGENG_ASSERT(!m_Instance, "Application already exists");

            m_Instance = std::make_unique<T>(args...);
            m_Instance->SetupApplication();

            try
            {
                m_Instance->Run();
            }
            catch (const std::exception& e)
            {
                LGENG_ERROR_TRACE("Uncaught exception! {}", e.what());
                return EXIT_FAILURE;
            }

            // TODO: convert app to own resources so this can go in Destroy
            IO::Logger::DestroyGlobalLogger();

            return EXIT_SUCCESS;
        }

#ifdef LGENG_HEADLESS
        template<typename T, typename... Args>
            requires std::is_base_of_v<Application, T>
        static Application& CreateHeadless(Args&&... args)
        {
            LGENG_ASSERT(!m_Instance, "Application already exists");

            m_Instance = std::make_unique<T>(args...);
            m_Instance->SetupApplication();

            return *m_Instance;
        }
#endif

        static void Destroy();

        static bool HasConstructed();
        static Application& Get();
    protected:
        virtual void OnSetup() = 0;

        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender(float deltaTime) {}
    private:
        void SetupApplication();
        void Run();

        // Must be called on the main thread
        void RenderFrame(float delta = 1.0f);

        static void RecalculateTransforms(Scene& scene);

        Events::EventBus m_EventBus;

#ifndef LGENG_HEADLESS
        Graphics::GraphicsContext& CreateGraphicsContext(std::string_view applicationName,
            GraphicsAPI api);
        Graphics::RenderTarget& CreateRenderTarget(int width, int height,
            std::string_view applicationName);
        Graphics::Renderer& CreateRenderer();

        Scope<Graphics::GraphicsContext> m_ManagedGraphicsContext = nullptr;
        Scope<Graphics::WindowRenderTarget> m_WindowRenderTarget = nullptr;
        Scope<Graphics::Renderer> m_ManagedRenderer = nullptr;

        Scope<Utils::Window> m_Window = nullptr;

        Graphics::RenderTarget& m_RenderTarget;
        Graphics::Renderer& m_Renderer;
#endif

        Graphics::GraphicsContext& m_GraphicsContext;

        Scene m_GlobalScene;
        Scene* m_pActiveScene = nullptr;

        Objects::Camera* m_pActiveCamera = nullptr;

        bool m_IsSetUp = false;
        bool m_Headless = false;

        static Scope<Application> m_Instance;
    };
}
