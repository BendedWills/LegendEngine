#ifndef _LEGENDENGINE_SCENE3D_HPP
#define _LEGENDENGINE_SCENE3D_HPP

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Object3d/Object.hpp>
#include <LegendEngine/Object3d/Components/MeshComponent.hpp>

namespace LegendEngine
{
    class Application3D;
    class Scene3D
    {
        friend Application3D;
    public:
        Scene3D() {}
        LEGENDENGINE_NO_COPY(Scene3D);

        /**
         * @brief Adds an object to the scene.
         * 
         * @param object The object to add.
         */
        void AddObject(Object3d::Object& object);
        /**
         * @brief Adds an object to the scene.
         * 
         * @param pObject The object to add.
         */
        void AddObject(Object3d::Object* pObject);
        /**
         * @returns True if an object is in the scene; otherwise, false.
         */
        bool HasObject(Object3d::Object& object);
        /**
         * @returns True if an object is in the scene; otherwise, false.
         */
        bool HasObject(Object3d::Object* pObject);
        /**
         * @brief Removes an object from the scene.
         * 
         * @param object The object to remove.
         * 
         * @returns True if the object was successfully removed;
         *  otherwise, false.
         */
        bool RemoveObject(Object3d::Object& object);
        /**
         * @brief Removes an object from the scene.
         * 
         * @param pObject The object to remove.
         * 
         * @returns True if the object was successfully removed;
         *  otherwise, false.
         */
        bool RemoveObject(Object3d::Object* pObject);
    protected:
        Application3D* pApplication = nullptr;
    private:
        // Commented because it might be unnessesary and extra work is required.
        // void AddComponents(Object3d::Object* pObject);
        // void RemoveComponents(Object3d::Object* pObject);

        std::vector<Object3d::Object*> objects;
        // std::vector<Object3d::Components::MeshComponent*> meshComponents;
    };
}

#endif //_LEGENDENGINE_SCENE3D_HPP