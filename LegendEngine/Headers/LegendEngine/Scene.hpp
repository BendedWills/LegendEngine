#ifndef _LEGENDENGINE_SCENE3D_HPP
#define _LEGENDENGINE_SCENE3D_HPP

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/IApplicationHolder.hpp>
#include <LegendEngine/Objects/Object.hpp>
#include <LegendEngine/Objects/Components/MeshComponent.hpp>

namespace LegendEngine
{
    class Scene : public IApplicationHolder
    {
        friend Objects::Object;
    public:
        LEGENDENGINE_NO_COPY(Scene);
        
        Scene() {}

        /**
         * @brief Adds an object to the scene.
         *
         * @param object The object to add.
         */
        void AddObject(Ref<Objects::Object>& object);
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
        bool HasObject(Ref<Objects::Object>& object);
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
        bool RemoveObject(Ref<Objects::Object>& object);
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
		std::unordered_map<std::string,
			std::vector<Objects::Components::Component*>>* GetObjectComponents();
    protected:
        void OnObjectComponentAdd(Objects::Object* pObject, std::string typeName, 
            Objects::Components::Component* pComponent);
        void OnObjectComponentRemove(Objects::Object* pObject, std::string typeName,
            Objects::Components::Component* pComponent);
		void OnObjectEnable(Objects::Object* pObject);
		void OnObjectDisable(Objects::Object* pObject);
    private:
        void AddObjectComponents(Objects::Object* pObject);
        void RemoveObjectComponents(Objects::Object* pObject);

        std::vector<Objects::Object*> objects;
		std::unordered_map<std::string,
std::vector<Objects::Components::Component*>>
			components;
    };
}

#endif //_LEGENDENGINE_SCENE3D_HPP