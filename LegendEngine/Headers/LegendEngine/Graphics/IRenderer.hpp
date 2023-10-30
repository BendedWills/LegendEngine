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
    class Scene;
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

    class IRenderer
    {
        friend Application;
        friend Scene;
        friend Objects::IObjectNative;
        friend Resources::Material;
    public:
        IRenderer(Application& application);
        ~IRenderer();
        LEGENDENGINE_NO_COPY(IRenderer);

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

        /**
         * @brief
         * An object's native is a class that has
         * specific utilities native to the Application's graphics API.
         * It's called a native because I have literally no clue what else
         * to call it.
         *
         */
        virtual bool CreateObjectNative(Objects::Object* pObject) = 0;
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

        /**
         * @brief
         * Called when a resource changes in any way
         */
        virtual void OnResourceChange(Resources::IResource* pResource) {}

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

        virtual void OnWindowResize(uint64_t width, uint64_t height) {}

        /**
         * @brief 
         * Called when an object that is in the active scene or the default
         * scene changes in any way. 
         * 
         * This function is called when:
		 * - The object's material changes (or when the texture changes)
		 * - A component is added to the object
		 * - A component is removed from the object
         * 
         * @param pObject A pointer to the object that has changed.
         */
        virtual void OnObjectChange(Objects::Object* pObject) {}
        /**
         * @brief
         * Called when the default or active scene changes in any way.
         * 
         * This function is called when:
         * - An object is added to the scene
         * - An object is removed
         * - A new active scene is set
         * 
         * @param pScene The scene that has changed.
         * @param pObject The object that has changed. This parameter is only
         *  present when an object is added or removed from the scene
         */
        virtual void OnSceneChange(Scene* pScene, Objects::Object* pObject) {}

        virtual void OnRendererDispose() {}

        Ref<Resources::Material> defaultMaterial;
    private:
        void OnDispose();

        Application& m_Application;
    };
}

#endif //_LEGENDENGINE_IRENDERER_HPP