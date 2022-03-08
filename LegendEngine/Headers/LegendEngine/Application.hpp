/*
 * The Application class is an interface to be implemented by other types of
 * applications.
 * Using this class as an interface is optional.
 */

#ifndef _LEGENDENGINE_APPLICATION_HPP
#define _LEGENDENGINE_APPLICATION_HPP

#include <string>

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/Stopwatch.hpp>
#include <LegendEngine/Graphics/Vulkan/Instance.hpp>
#include <LegendEngine/Scene.hpp>

#include <Tether/Tether.hpp>

namespace LegendEngine::Objects
{
    class Object;
}

namespace LegendEngine
{
    class IRenderer;
    class Application : public IDisposable
    {
        friend Scene;
    public:
        Application() 
        {}
        LEGENDENGINE_NO_COPY(Application);

        /**
         * @brief Initializes the application
         * 
         * @param applicationName The name of the application.
         * @param logging If the application should log.
         * @param debug If the application should log debug/verbose 
         *  information.
         * @param api The rendering api to use.
         * 
         * @returns True if initialization was successful; otherwise, false.
         */
        bool Start(
            const std::string& applicationName,
            bool logging = false, 
            bool debug = false,
            RenderingAPI api = RenderingAPI::AUTO_SELECT
        );

        bool InitVulkan(bool enableValidationLayers = false);

        /**
         * @brief Sets the application renderer.
         *  Note: The renderer's application must be equivalent to this application.
         * 
         * @param pRenderer A pointer to the renderer.
         * 
         * @returns If the renderer is initialized. The renderer is required to
         *  be initialized to be used in the application.
         */
        bool SetRenderer(IRenderer* pRenderer);
        IRenderer* GetRenderer();

        Application* Get();
        std::string GetName();
        Tether::IWindow* GetWindow();
        bool IsCloseRequested();
        bool IsApiInitialized();

        // Window Management
        void SetFullscreen(bool fullscreen, int monitor = 0);
        
        // Vulkan functions
        bool IsVulkanInitialized();
        Vulkan::Instance* GetVulkanInstance();

        /**
         * @param message The message to log
         * @param type The type of log.
         */
        void Log(const std::string& message, LogType type);

        /**
         * @brief Initializes an object to this application.
         *
         * @param object The object to initialize.
         *
         * @returns True if the object was successfully initialized,
         *  false if the object is already initialized.
         */
        bool InitObject(Objects::Object& object);
        /**
         * @brief Initializes an object to this application.
         *
         * @param pObject A pointer to the object to initialize.
         *
         * @returns True if the object was successfully initialized,
         *  false if the object is already initialized.
         */
        bool InitObject(Objects::Object* pObject);
        /**
         * @brief Initializes a scene to this application.
         *
         * @param scene The scene to initialize.
         *
         * @returns True if the scene was successfully initialized,
         *  false if the scene is already initialized.
         */
        bool InitScene(Scene& scene);
        /**
         * @brief Initializes a scene to this application.
         *
         * @param pScene A pointer to the scene to initialize.
         *
         * @returns True if the object was successfully initialized,
         *  false if the object is already initialized.
         */
        bool InitScene(Scene* pScene);

        Scene* GetDefaultScene();

        /**
         * @brief Sets the active scene.
         *
         * @param scene The scene.
         */
        void SetActiveScene(Scene& scene);
        /**
         * @brief Sets the active scene.
         *
         * @param pScene A pointer to the scene.
         */
        void SetActiveScene(Scene* pScene);
        void RemoveActiveScene();
        /**
         * @returns The active scene (wow really?)
         */
        Scene* GetActiveScene();
    protected:
        /**
         * @brief Called before the Init function is executed.
         * 
         * @return Should return true if starting was successful;
         *  otherwise, false.
         */
        virtual bool OnPreInit() { return true; }
        /**
         * @brief Called after the Init function is called.
         * 
         * @return Should return true if starting was successful;
         *  otherwise, false.
         */
        virtual bool OnInit() { return true; }
        /**
         * @brief Called every frame before screen clearing.
         */
        virtual void OnPreUpdate() {}
        /**
         * @brief Called every frame
         */
        virtual void OnUpdate() {}
        /**
         * @brief Called before the application is disposed.
         */
        virtual void OnStop() {}
        /**
         * @brief Called after the application is disposed.
         */
        virtual void OnDisposed() {}
    protected:
        bool InitWindow(const std::string& title);

        bool StartLoop();
        void UpdateWindow();
        void Update(bool updateWindow = true);
        void Render();

        void DisposeGraphics();
        
        void OnDispose();

        Tether::SimpleWindow window;
        IRenderer* pRenderer = nullptr;

        std::string applicationName = "";
        bool logging = false;
        bool debug = false;

    #pragma region Graphics

        bool initializedApi = false;

        bool initializedVulkan = false;
        Vulkan::Instance instance;

        class DebugCallback : public Vulkan::DebugCallback
        {
        public:
            Application* pApplication = nullptr;

            void OnDebugLog(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
            );
        };
        DebugCallback callback;
    
    #pragma endregion Graphics

        void OnSceneObjectAdd(Scene* pScene, Objects::Object* pObject);
        void OnSceneObjectRemove(Scene* pScene, Objects::Object* pObject);
    private:
        // Every application has a default scene. This scene contains objects
        // that are always rendered, no matter what the current set scene is.
        Scene defaultScene;
        Scene* activeScene;
    };
}

#endif //_LEGENDENGINE_Application_HPP