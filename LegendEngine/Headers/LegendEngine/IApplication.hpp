/*
 * The IApplication class is an interface to be implemented by other types of
 * applications.
 * Using this class as an interface is optional.
 */

#ifndef _LEGENDENGINE_IAPPLICATION_HPP
#define _LEGENDENGINE_IAPPLICATION_HPP

#include <string>

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/Stopwatch.hpp>
#include <LegendEngine/Graphics/Vulkan/Instance.hpp>

#include <Tether/Tether.hpp>

namespace LegendEngine
{
    class IRenderer;
    class IApplication : public IDisposable
    {
    public:
        IApplication() {}
        LEGENDENGINE_NO_COPY(IApplication);

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

        IApplication* Get();
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

        virtual bool OnAppInit() { return true; }
        virtual void OnAppStop() {}
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
            IApplication* pApplication = nullptr;

            void OnDebugLog(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
            );
        };
        DebugCallback callback;
    
    #pragma endregion Graphics
    };
}

#endif //_LEGENDENGINE_IAPPLICATION_HPP