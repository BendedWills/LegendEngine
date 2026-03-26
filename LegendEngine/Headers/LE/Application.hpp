#pragma once

#include <memory>
#include <LE/TetherBindings.hpp>
#include <LE/Common/Assert.hpp>
#include <LE/Common/Defs.hpp>
#include <LE/Events/EventBus.hpp>
#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/Graphics/Renderer.hpp>
#include <LE/IO/Logger.hpp>
#include <LE/Resources/ResourceManager.hpp>

namespace le
{
    enum class GraphicsAPI;

    class Application final
    {
    public:
        static constexpr size_t FRAMES_IN_FLIGHT = 2;

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

        LE_NO_COPY(Application);

        void SetActiveCameraID(UID cameraID);
        void SetActiveScene(Scene& scene);

        void ClearActiveScene();

        GraphicsContext& GetGraphicsContext() const;
        RenderTarget& GetRenderTarget() const;
        EventBus& GetEventBus();
        ResourceManager& GetResourceManager();
        Scene& GetGlobalScene();
        UID GetActiveCameraID() const;
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

        static void RecalculateTransforms(Scene& scene);

        EventBus m_EventBus;
        ResourceManager m_resourceManager;

#ifndef LE_HEADLESS
        class ResizeHandler : public Tether::Events::EventHandler
        {
        public:
            explicit ResizeHandler(Application& app);

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

        Scope<Window> m_Window = nullptr;
#endif
        GraphicsContext& m_GraphicsContext;
#ifndef LE_HEADLESS
        RenderTarget& m_RenderTarget;
        Renderer& m_Renderer;
#endif

        Scene m_GlobalScene;
        Scene* m_pActiveScene = nullptr;

        UID m_activeCameraID = 0;

        bool m_Headless = false;

        static Scope<Application> m_Instance;
    };
}
