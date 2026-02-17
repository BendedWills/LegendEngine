#pragma once

#include <memory>
#include <LegendEngine/Scene.hpp>
#include <LegendEngine/Common/Logger.hpp>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Events/EventBus.hpp>
#include <LegendEngine/Graphics/GraphicsContext.hpp>
#include <LegendEngine/Graphics/Renderer.hpp>
#include <LegendEngine/Graphics/RenderTarget.hpp>
#include <LegendEngine/Objects/Camera.hpp>
#include <Tether/Window.hpp>

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
            bool logging,
            bool debug,
            GraphicsAPI api);
#endif
        Application(
            std::string_view applicationName,
            bool logging,
            bool debug,
            Graphics::GraphicsContext& graphicsContext,
            Graphics::Renderer& renderer);
        virtual ~Application() = 0;

        LEGENDENGINE_NO_COPY(Application);

        void SetActiveCamera(Objects::Camera* pCamera);
        void SetActiveScene(Scene& scene);

        void ClearActiveScene();

        Graphics::GraphicsContext& GetGraphicsContext() const;
        Graphics::RenderTarget& GetRenderTarget() const;
        Events::EventBus& GetEventBus();
        Scene& GetGlobalScene();
        [[nodiscard]] Graphics::Renderer& GetRenderer() const;
        Objects::Camera* GetActiveCamera() const;
        Scene* GetActiveScene() const;
        Logger& GetLogger();

        template<typename T, typename... Args>
            requires std::is_base_of_v<Application, T>
        static int RunApplication(Args&&... args)
        {
            if (m_Instance)
                return EXIT_FAILURE;

            m_Instance = std::make_unique<T>(args...);
            m_Instance->SetupApplication();

            try
            {
                m_Instance->Run();
            }
            catch (const std::exception& e)
            {
                m_Instance->m_Logger.Log(Logger::Level::ERROR,
                    "Uncaught exception!");
                m_Instance->m_Logger.Log(Logger::Level::ERROR,
                    e.what());
                return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        }

        template<typename T, typename... Args>
            requires std::is_base_of_v<Application, T>
        static Application& CreateHeadless(Args&&... args)
        {
            if (m_Instance)
                throw std::logic_error("Application already exists");

            m_Instance = std::make_unique<T>(args...);
            m_Instance->SetupApplication();

            return *m_Instance;
        }

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

        void Update(float delta, bool updateWindow = true);
        void Render(float delta);

        static void RecalculateTransforms(Scene& scene);

        Events::EventBus m_EventBus;

        Graphics::GraphicsContext& m_GraphicsContext;
        Graphics::RenderTarget& m_RenderTarget;
        Graphics::Renderer& m_Renderer;

        Logger m_Logger;

        Scene m_GlobalScene;
        Scene* m_pActiveScene = nullptr;

        Objects::Camera* m_pActiveCamera = nullptr;

        bool m_Debug = false;
        bool m_IsSetUp = false;
        bool m_Headless = false;

#ifndef LGENG_HEADLESS
        Scope<Graphics::GraphicsContext> m_ManagedGraphicsContext = nullptr;
        Scope<Graphics::WindowRenderTarget> m_WindowRenderTarget = nullptr;
        Scope<Graphics::Renderer> m_ManagedRenderer = nullptr;
#endif

        static Scope<Application> m_Instance;
    };
}
