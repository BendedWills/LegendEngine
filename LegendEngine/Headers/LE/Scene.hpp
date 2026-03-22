#pragma once

#include <memory>
#include <shared_mutex>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <LE/Common/Types.hpp>
#include <LE/Common/UID.hpp>
#include <LE/Components/Component.hpp>

namespace le
{
    class Scene final
    {
        friend class Object;
    public:
        using ObjectsType = std::unordered_map<UID, Scope<Object>>;
        using ComponentsVecType = std::unordered_map<std::type_index,
            std::vector<Component*>>;

        Scene() = default;
        LE_NO_COPY(Scene);

        Object CreateObject();

        bool HasObject(const Object& object);
        bool HasObject(UID objectID);
        void RemoveObject(Object& toRemove);
        void RemoveObject(UID toRemove);

        void Clear();

        ComponentsVecType GetObjectComponents();

        static Scope<Scene> Create();
    protected:
        void OnComponentAdd(std::type_index id, Component& component);
        void OnComponentRemove(std::type_index id, Component& component);
    private:
        Object* AddObject(Scope<Object> object);

        void AddObjectComponents(Object& object);
        void RemoveObjectComponents(Object& object);

        std::shared_mutex m_ComponentsMutex;
        ComponentsVecType m_Components;
    };
}
