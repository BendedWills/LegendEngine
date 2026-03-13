#pragma once

#include <LegendEngine/Objects/Object.hpp>
#include <LegendEngine/Events/EventBus.hpp>
#include <LegendEngine/Events/ComponentAddedEvent.hpp>
#include <LegendEngine/Events/ComponentRemovedEvent.hpp>

#include <memory>
#include <typeindex>
#include <vector>

namespace le
{
    class Scene final
    {
    public:
        using ComponentsVecType = std::unordered_map<std::type_index,
            std::vector<Component*>>;

        Scene();
        explicit Scene(EventBus& eventBus);
        LEGENDENGINE_NO_COPY(Scene);

        template <typename T, typename... Args>
            requires std::is_base_of_v<Object, T>
        T* CreateObject(Args&&... args)
        {
            return reinterpret_cast<T*>(AddObject(std::make_unique<T>(std::forward<Args>(args)...)));
        }

        bool HasObject(const Object& object) const;
        bool HasObject(UID objectID) const;
        void RemoveObject(Object& toRemove);

        void Clear();

        const std::vector<Scope<Object>>& GetObjects() const;
        ComponentsVecType& GetObjectComponents();

        static Scope<Scene> Create();
    private:
        Object* AddObject(Scope<Object> object);

        void ListenForEvents();

        void AddObjectComponents(Object& object);
        void RemoveObjectComponents(Object& object);

        void OnComponentAdd(const ComponentAddedEvent& event);
        void OnComponentRemove(const ComponentRemovedEvent& event);

        std::vector<Scope<Object>> m_Objects;
        ComponentsVecType m_Components;

        EventBusSubscriber m_EventSubscriber;
    };
}
