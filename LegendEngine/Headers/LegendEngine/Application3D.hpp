#ifndef _LEGENDENGINE_APPLICATION3D_HPP
#define _LEGENDENGINE_APPLICATION3D_HPP

#include <LegendEngine/IApplication.hpp>
#include <LegendEngine/Common/Ref.hpp>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Scene3D.hpp>

namespace LegendEngine
{
    class Scene3D;
    class Application3D : public IApplication
    {
        friend class Scene3D;
    public:
        Application3D() {}
        LEGENDENGINE_NO_COPY(Application3D);

        /**
         * @brief Initializes an object to this application.
         * 
         * @param object The object to initialize.
         *
         * @returns True if the object was successfully initialized,
         *  false if the object is already initialized.
         */
        bool InitObject(Object3d::Object& object);
        /**
         * @brief Initializes an object to this application.
         * 
         * @param pObject A pointer to the object to initialize.
         *
         * @returns True if the object was successfully initialized,
         *  false if the object is already initialized.
         */
        bool InitObject(Object3d::Object* pObject);
        /**
         * @brief Initializes a scene to this application.
         * 
         * @param scene The scene to initialize.
         *
         * @returns True if the scene was successfully initialized,
         *  false if the scene is already initialized.
         */
        bool InitScene(Scene3D& scene);
        /**
         * @brief Initializes a scene to this application.
         * 
         * @param pScene A pointer to the scene to initialize.
         *
         * @returns True if the object was successfully initialized,
         *  false if the object is already initialized.
         */
        bool InitScene(Scene3D* pScene);

        Scene3D* GetDefaultScene();

        /**
         * @brief Sets the active scene.
         * 
         * @param scene The scene.
         */
        void SetActiveScene(Scene3D& scene);
        /**
         * @brief Sets the active scene.
         * 
         * @param pScene A pointer to the scene.
         */
        void SetActiveScene(Scene3D* pScene);
        void RemoveActiveScene();
        /**
         * @returns The active scene (wow really?)
         */
        Scene3D* GetActiveScene();
    protected:
        void OnSceneObjectAdd(Scene3D* pScene, Object3d::Object* pObject);
        void OnSceneObjectRemove(Scene3D* pScene, Object3d::Object* pObject);
    private:
        bool OnAppInit();

        // Every application has a default scene. This scene contains objects
        // that are always rendered, no matter what the current set scene is.
        Scene3D defaultScene;
        Scene3D* activeScene;
    };
}

#endif //_LEGENDENGINE_APPLICATION3D_HPP