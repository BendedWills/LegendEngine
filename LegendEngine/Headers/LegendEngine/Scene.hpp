#ifndef _LEGENDENGINE_SCENE3D_HPP
#define _LEGENDENGINE_SCENE3D_HPP

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Objects/Object.hpp>
#include <LegendEngine/Objects/Components/MeshComponent.hpp>

namespace LegendEngine
{
    class Application;
    class Scene
    {
        friend Application;
        friend Objects::Object;
    public:
        Scene() {}
        LEGENDENGINE_NO_COPY(Scene);

        /**
         * @brief Adds an object to the scene.
         * 
         * @param object The object to add.
         */
        void AddObject(Objects::Object& object);
        /**
         * @brief Adds an object to the scene.
         * 
         * @param pObject The object to add.
         */
        void AddObject(Objects::Object* pObject);
        /**
         * @returns True if an object is in the scene; otherwise, false.
         */
        bool HasObject(Objects::Object& object);
        /**
         * @returns True if an object is in the scene; otherwise, false.
         */
        bool HasObject(Objects::Object* pObject);
        /**
         * @brief Removes an object from the scene.
         * 
         * @param object The object to remove.
         * 
         * @returns True if the object was successfully removed;
         *  otherwise, false.
         */
        bool RemoveObject(Objects::Object& object);
        /**
         * @brief Removes an object from the scene.
         * 
         * @param pObject The object to remove.
         * 
         * @returns True if the object was successfully removed;
         *  otherwise, false.
         */
        bool RemoveObject(Objects::Object* pObject);

        void ClearObjects();

        std::vector<Objects::Object*>* GetObjects();
    protected:
        void OnObjectComponentAdd(Objects::Object*, std::string typeName, 
            Objects::Components::Component* pComponent);
        void OnObjectComponentRemove(Objects::Object*, std::string typeName,
            Objects::Components::Component* pComponent);

        Application* pApplication = nullptr;
    private:
        // Commented because it might be unnessesary and extra work is required.
        // void AddComponents(Objects::Object* pObject);
        // void RemoveComponents(Objects::Object* pObject);

        std::vector<Objects::Object*> objects;
        // std::vector<Objects::Components::MeshComponent*> meshComponents;
    };
}

#endif //_LEGENDENGINE_SCENE3D_HPP