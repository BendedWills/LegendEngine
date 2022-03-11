#ifndef _LEGENDENGINE_IRENDERER_HPP
#define _LEGENDENGINE_IRENDERER_HPP

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Common/Ref.hpp>

#include <vector>

namespace LegendEngine::Objects
{
    class Object;

    namespace Components
    {
        class Component;
    }
}

namespace LegendEngine
{
    class VertexBuffer;
    class Application;
    class Scene;

    class IRenderer : public IDisposable
    {
        friend Application;
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
        bool Init(Application* pApplication);

        void SetVSyncEnabled(bool vsync);
        
        virtual bool CreateVertexBuffer(Ref<VertexBuffer>* buffer) 
        { return false; }

        Application* GetApplication();
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
        virtual void OnSceneChange(Scene* pScene) {}
        /**
         * @brief Called when the application's active scene gains and object.
         */
        virtual void OnSceneObjectAdd(Scene* pScene, 
            Objects::Object* pObject) {}
        /**
         * @brief Called when the application's active scene loses and object.
         */
        virtual void OnSceneObjectRemove(Scene* pScene, 
            Objects::Object* pObject) {}
        /**
         * @brief Called when the application's active scene is removed.
         */
        virtual void OnSceneRemove(Scene* pScene) {}

        /**
         * @brief Called when the application's default scene gains and object.
         */
        virtual void OnDefaultObjectAdd(Scene* pScene, 
            Objects::Object* pObject) {}
        /**
         * @brief Called when the application's default scene loses and object.
         */
        virtual void OnDefaultObjectRemove(Scene* pScene, 
            Objects::Object* pObject) {}
        /**
         * @brief Called when an object that is in either the Application's 
         *  active scene or default scene has a component added to it.
         * 
         * @param pScene The scene of the object
         * @param pObject The object.
         * @param typeName The component type name.
         * @param pComponent The component.
         */
        virtual void OnSceneObjectComponentAdd(
            Scene* pScene,
            Objects::Object* pObject, 
            const std::string& typeName, 
            Objects::Components::Component* pComponent) 
        {}
        /**
         * @brief Called when an object that is in either the Application's 
         *  active scene or default scene has a component removed from it.
         * 
         * @param pScene The scene of the object
         * @param pObject The object.
         * @param typeName The component type name.
         * @param pComponent The component.
         */
        virtual void OnSceneObjectComponentRemove(
            Objects::Object* pObject,
            const std::string& typeName, 
            Objects::Components::Component* pComponent) 
        {}

        /**
         * @brief Called when the renderer is desposed.
         */
        virtual void OnRendererDispose() {}

        bool enableVsync = false;

        std::vector<VertexBuffer*> vertexBuffers;
    private:
        void OnDispose();

        Application* pApplication = nullptr;
    };
}

#endif //_LEGENDENGINE_IRENDERER_HPP