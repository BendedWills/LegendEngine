#ifndef _LEGENDENGINE_IRENDERER_HPP
#define _LEGENDENGINE_IRENDERER_HPP

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Application.hpp>

namespace LegendEngine
{
    class IRenderer : public IDisposable
    {
        friend Application;
    public:
        IRenderer() {}
        
        IRenderer(const IRenderer&) = delete;
		IRenderer(IRenderer&&) = delete;
		IRenderer& operator=(const IRenderer&) = delete;
		IRenderer& operator=(IRenderer&&) = delete;

        /**
         * @brief Initializes the renderer.
         * 
         * @param pApplication A pointer to the application that this renderer
         *  should be created for.
         * 
         * @returns True if the renderer was successfully initialized;
         *  Otherwise, false.
         */
        bool Init(Application* pApplication);

        Application* GetApplication();
    protected:
        /**
         * @brief Called when the renderer is initialized.
         * 
         * @returns True if the renderer was successfully initialized;
         *  Otherwise, false.
         */
        virtual bool OnRendererInit() = 0;
        /**
         * @brief Called when the renderer is desposed.
         */
        virtual void OnRendererDispose() = 0;
    private:
        void OnDispose();

        Application* pApplication = nullptr;
    };
}

#endif //_LEGENDENGINE_IRENDERER_HPP