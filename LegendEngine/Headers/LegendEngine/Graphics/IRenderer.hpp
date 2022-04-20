#ifndef _LEGENDENGINE_IRENDERER_HPP
#define _LEGENDENGINE_IRENDERER_HPP

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Common/Ref.hpp>

#include <LegendEngine/Resources/Shader.hpp>
#include <LegendEngine/Resources/Texture2D.hpp>
#include <LegendEngine/Resources/Material.hpp>

#include <vector>
#include <string>

namespace LegendEngine
{
	namespace Objects
	{
		class Object;
		class IObjectNative;

		namespace Components
		{
			class Component;
		}
	}

    class VertexBuffer;
    class Application;
    class Scene;

    class IRenderer : public IDisposable
    {
        friend Application;
        friend Objects::IObjectNative;
    public:
        LEGENDENGINE_DISPOSE_ON_DESTRUCT(IRenderer);
        
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

        virtual void SetVSyncEnabled(bool vsync) {}
        virtual bool CreateShaderNative(Resources::Shader* pShader) 
        { return false;  }
		virtual bool CreateTexture2DNative(Resources::Texture2D* pTexture)
		{ return false; }
		virtual bool CreateMaterialNative(Resources::Material* pMaterial)
		{ return false; }

        /**
         * @brief An VertexBuffer's native is a class that has
         *  specific utilities native to the Application's graphics API.
         */
        virtual bool CreateVertexBufferNative(LegendEngine::VertexBuffer* pObject) = 0;

        Application* GetApplication();
    protected:
        IRenderer() {}

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
            Scene* pScene,
            Objects::Object* pObject,
            const std::string& typeName, 
            Objects::Components::Component* pComponent) 
        {}

		virtual void OnSceneObjectEnable(Scene* pScene, Objects::Object* pObject) 
        {}
        virtual void OnSceneObjectDisable(Scene* pScene, Objects::Object* pObject)
        {}

		virtual void OnWindowResize()
		{}

        /**
         * @brief Called when the renderer is disposed.
         */
        virtual void OnRendererDispose() {}

        /**
         * @brief An object's native is a class that has
         *  specific utilities native to the Application's graphics API.
         *  It's called a native because I have literally no clue what else to call it.
         */
        virtual bool CreateObjectNative(Objects::Object* pObject) = 0;
    private:
        void OnDispose();

        Application* pApplication = nullptr;
    };
}

#endif //_LEGENDENGINE_IRENDERER_HPP