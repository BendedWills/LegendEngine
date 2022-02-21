#ifndef _LEGENDENGINE_IRENDERER_HPP
#define _LEGENDENGINE_IRENDERER_HPP

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/IApplication.hpp>

namespace LegendEngine
{
    class IRenderer : public IDisposable
    {
        friend IApplication;
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
         *  otherwise, false.
         */
        bool Init(IApplication* pApplication);

        void SetVSyncEnabled(bool vsync);

        IApplication* GetApplication();
    protected:
        bool RenderFrame() { return OnRenderFrame(); }

        /**
         * @brief Called when a frame should be rendered
         * 
         * @returns True if the frame was successfully rendered;
         *  otherwise, false.
         */
        virtual bool OnRenderFrame() = 0;

        /**
         * @brief Called when the renderer is initialized.
         * 
         * @returns True if the renderer was successfully initialized;
         *  otherwise, false.
         */
        virtual bool OnRendererInit() = 0;
        /**
         * @brief Called when the renderer is desposed.
         */
        virtual void OnRendererDispose() = 0;

        bool enableVsync = false;
    private:
        void OnDispose();

        IApplication* pApplication = nullptr;
    };
}

#endif //_LEGENDENGINE_IRENDERER_HPP