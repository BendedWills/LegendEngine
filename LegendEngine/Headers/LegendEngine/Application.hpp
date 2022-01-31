/*
 * The Application class is an interface to be implemented as the application.
 * Using this class as an interface is optional.
 */

#ifndef _LEGENDENGINE_APPLICATION_HPP
#define _LEGENDENGINE_APPLICATION_HPP

#include <string>

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Common/Types.hpp>

#include <Tether/Tether.hpp>

namespace LegendEngine
{
    class IRenderer;
    class Application : public IDisposable
    {
    public:
        Application() {}
        
        Application(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

        /**
         * @brief Initializes the application
         * 
         * @param applicationName The name of the application.
         * @param logging If the application should log.
         * @param debug If the application should log debug/verbose 
         *  information.
         * 
         * @returns True if initialization was successful; otherwise, false.
         */
        bool Init(
            const std::string& applicationName,
            bool logging = false, 
            bool debug = false
        );

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

        Application* Get();
        std::string GetName();
        Tether::IWindow* GetWindow();
        bool IsCloseRequested();
        
        /**
         * @brief Updates and renders the application.
         * 
         * @returns True if processing was successful; otherwise, false.
         */
        bool ProcessFrame();

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
         * @brief Called when the application is disposed.
         */
        virtual void OnStop() {}
    private:
        bool InitWindow(const std::string& title);
        
        void Update();
        void Render();

        void OnDispose();

        Tether::SimpleWindow window;
        IRenderer* pRenderer = nullptr;

        std::string applicationName = "";
        bool logging = false;
        bool debug = false;
    };
}

#endif //_LEGENDENGINE_IAPPLICATION_HPP