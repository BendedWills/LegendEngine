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

namespace le
{
    namespace Input = Tether::Input;
    namespace Utils = Tether;

    class Application final
    {
    public:
        class Listener
        {
        public:
            virtual ~Listener() = default;

            virtual void OnSetup() = 0;

            virtual void OnUpdate(float delta) {}
            virtual void OnRender(float delta) {}
        };

#ifndef LE_HEADLESS
        // Creates the Application with a WindowRenderTarget
        Application(
            int width, int height,
            std::string_view applicationName,
            GraphicsAPI api);

        [[nodiscard]] Renderer& GetRenderer() const;
        Tether::Window& GetWindow() const;
#else
        Application(GraphicsContext& ctx); // Headless application constructor
#endif
        ~Application();

        LEGENDENGINE_NO_COPY(Application);

        void SetActiveCamera(Camera* pCamera);
        void SetActiveScene(Scene& scene);

        void ClearActiveScene();

        GraphicsContext& GetGraphicsContext() const;
        RenderTarget& GetRenderTarget() const;
        EventBus& GetEventBus();
        Scene& GetGlobalScene();
        Camera* GetActiveCamera() const;
        Scene* GetActiveScene() const;

        void Update(float delta, bool updateWindow = true);
        void Render(float delta);

        template<typename... Args>
        static void Create(Args&&... args)
        {
            LE_ASSERT(!m_Instance, "Application already exists");
            m_Instance = std::make_unique<Application>(args...);
        }

        static void Run();
        static void Destroy();

        static bool HasConstructed();
        static Application& Get();
    private:
        void RunInstance();

        // Must be called on the main thread
        void RenderFrame(float delta = 1.0f);

        static void RecalculateTransforms(const Scene& scene);

        EventBus m_EventBus;

#ifndef LE_HEADLESS
        class ResizeHandler : public Tether::Events::EventHandler
        {
        public:
            ResizeHandler(Application& app);

            void OnWindowResize(const Tether::Events::WindowResizeEvent& event) override;

            Application& m_Application;
        };
        ResizeHandler m_ResizeHandler;

        GraphicsContext& CreateGraphicsContext(std::string_view applicationName,
            GraphicsAPI api);
        RenderTarget& CreateRenderTarget(int width, int height,
            std::string_view applicationName);
        Renderer& CreateRenderer();

        Scope<GraphicsContext> m_ManagedGraphicsContext = nullptr;
        Scope<RenderTarget> m_WindowRenderTarget = nullptr;
        Scope<Renderer> m_ManagedRenderer = nullptr;

        Scope<Utils::Window> m_Window = nullptr;
#endif
        GraphicsContext& m_GraphicsContext;
#ifndef LE_HEADLESS
        RenderTarget& m_RenderTarget;
        Renderer& m_Renderer;
#endif

        Scene m_GlobalScene;
        Scene* m_pActiveScene = nullptr;

        Camera* m_pActiveCamera = nullptr;

        bool m_Headless = false;

        static Scope<Application> m_Instance;
    };
}
