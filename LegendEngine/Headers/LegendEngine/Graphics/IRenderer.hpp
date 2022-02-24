#ifndef _LEGENDENGINE_IRENDERER_HPP
#define _LEGENDENGINE_IRENDERER_HPP

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Common/Ref.hpp>

#include <vector>

namespace LegendEngine::Object3d
{
    class Object;
}

namespace LegendEngine
{
    class VertexBuffer;
    class IApplication;
    class Application3D;
    class Scene3D;

    class IRenderer : public IDisposable
    {
        friend IApplication;
        friend Application3D;
        friend LegendEngine::VertexBuffer;
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
        
        virtual bool CreateVertexBuffer(Ref<VertexBuffer>* buffer) 
        { return false; }

        IApplication* GetApplication();
    protected:
        bool RenderFrame() { return OnRenderFrame(); }

        /**
         * @brief Called when a frame should be rendered
         * 
         * @returns True if the frame was successfully rendered;
         *  otherwise, false.
         */
        virtual bool OnRenderFrame() { return true; }

        /**
         * @brief Called when the renderer is initialized.
         * 
         * @returns True if the renderer was successfully initialized;
         *  otherwise, false.
         */
        virtual bool OnRendererInit() { return true; }

        /**
         * @brief Called when the application's active scene is changed.
         */
        virtual void OnSceneChange(Scene3D* pScene) {}
        /**
         * @brief Called when the application's active scene gains and object.
         */
        virtual void OnSceneObjectAdd(Scene3D* pScene, 
            Object3d::Object* pObject) {}
        /**
         * @brief Called when the application's active scene loses and object.
         */
        virtual void OnSceneObjectRemove(Scene3D* pScene, 
            Object3d::Object* pObject) {}
        /**
         * @brief Called when the application's active scene is removed.
         */
        virtual void OnSceneRemove(Scene3D* pScene) {}

        /**
         * @brief Called when the application's default scene gains and object.
         */
        virtual void OnDefaultObjectAdd(Scene3D* pScene, 
            Object3d::Object* pObject) {}
        /**
         * @brief Called when the application's default scene loses and object.
         */
        virtual void OnDefaultObjectRemove(Scene3D* pScene, 
            Object3d::Object* pObject) {}

        /**
         * @brief Called when the renderer is desposed.
         */
        virtual void OnRendererDispose() {}

        bool enableVsync = false;

        std::vector<VertexBuffer*> vertexBuffers;
    private:
        void OnDispose();

        IApplication* pApplication = nullptr;
    };
}

#endif //_LEGENDENGINE_IRENDERER_HPP