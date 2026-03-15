#pragma once

#include <LegendEngine/Objects/Object.hpp>
#include <LegendEngine/Events/EventBus.hpp>

#include <memory>
#include <typeindex>
#include <vector>
#include <shared_mutex>

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
        LEGENDENGINE_NO_COPY(Scene);

        template <typename T, typename... Args>
            requires std::is_base_of_v<Object, T>
        T* CreateObject(Args&&... args)
        {
            return reinterpret_cast<T*>(AddObject(std::make_unique<T>(std::forward<Args>(args)...)));
        }

        bool HasObject(const Object& object);
        bool HasObject(UID objectID);
        void RemoveObject(Object& toRemove);

        void Clear();

        const ObjectsType& GetObjects() const;
        ComponentsVecType GetObjectComponents();

        static Scope<Scene> Create();
    protected:
        void OnComponentAdd(std::type_index id, Component& component);
        void OnComponentRemove(std::type_index id, Component& component);
    private:
        Object* AddObject(Scope<Object> object);

        void AddObjectComponents(Object& object);
        void RemoveObjectComponents(Object& object);

        std::shared_mutex m_ObjectsMutex;
        ObjectsType m_Objects;

        std::shared_mutex m_ComponentsMutex;
        ComponentsVecType m_Components;
    };
}
