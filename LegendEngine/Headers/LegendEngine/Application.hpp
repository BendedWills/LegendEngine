#pragma once

#include <memory>

#include "Common/Types.hpp"
#include <LegendEngine/Graphics/Renderer.hpp>
#include <LegendEngine/Objects/Camera.hpp>
#include <Tether/Window.hpp>

#include <LegendEngine/Scene.hpp>
#include <LegendEngine/Common/Logger.hpp>
#include <LegendEngine/Events/EventBus.hpp>
#include <LegendEngine/Graphics/GraphicsContext.hpp>

namespace LegendEngine
{
    namespace Input = Tether::Input;
    namespace Utils = Tether;

    class Application
    {
    public:
        Application(
            std::string_view applicationName,
            bool logging,
            bool debug,
            GraphicsAPI api);
        virtual ~Application() = 0;

        LEGENDENGINE_NO_COPY(Application);

        void SetupApplication();

        void SetActiveCamera(Objects::Camera* pCamera);
        void SetActiveScene(Scene& scene);

        void ClearActiveScene();

        Graphics::GraphicsContext& GetGraphicsContext() const;
        Events::EventBus& GetEventBus();
        Scene& GetGlobalScene();
        [[nodiscard]] Tether::Window& GetWindow() const;
        [[nodiscard]] Graphics::Renderer& GetRenderer() const;
        Objects::Camera* GetActiveCamera() const;
        Scene* GetActiveScene() const;
        Logger& GetLogger();

        bool IsCloseRequested() const;

        template<typename T, typename... Args>
            requires std::is_base_of_v<Application, T>
        static int RunApplication(Args... args)
        {
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

        static Application& Get();
    protected:
        virtual void OnSetup() = 0;

        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender(float deltaTime) {}
        virtual void OnResize(int width, int height) {}
    private:
        void Run();

        // Must be called on the main thread
        void RenderFrame(float delta = 1.0f);

        void Update(float delta, bool updateWindow = true);
        void Render(float delta);

        static void RecalculateTransforms(Scene& scene);

        void ReceiveResize(uint64_t width, uint64_t height);

        class EventHandler final : public Utils::Events::EventHandler
        {
        public:
            explicit EventHandler(Application& application);
            void OnWindowResize(Utils::Events::WindowResizeEvent event) const;
        private:
            Application& m_Application;
        };
        EventHandler m_EventHandler;

        Events::EventBus m_EventBus;

        Scope<Graphics::GraphicsContext> m_GraphicsContext;
        Scope<Graphics::Renderer> m_Renderer = nullptr;
        Scope<Tether::Window> m_Window = nullptr;

        Logger m_Logger;

        Scene m_GlobalScene;
        Scene* m_pActiveScene = nullptr;

        Objects::Camera* m_pActiveCamera = nullptr;

        bool m_Debug = false;
        bool m_IsSetUp = false;

        static Scope<Application> m_Instance;
    };
}
