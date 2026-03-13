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
        using ObjectsVecType = std::vector<Object*>;
        using ComponentsVecType = std::unordered_map<std::type_index,
            std::vector<Component*>>;

        Scene();
        explicit Scene(EventBus& eventBus);
        ~Scene();

        LEGENDENGINE_NO_COPY(Scene);

        void AddObject(Object& object);
        bool HasObject(const Object& object);
        void RemoveObject(Object& object);

        void Clear();

        ObjectsVecType& GetObjects();
        ComponentsVecType& GetObjectComponents();

        static Scope<Scene> Create();
    private:
        void ListenForEvents();

        void AddObjectComponents(Object& object);
        void RemoveObjectComponents(Object& object);

        void OnComponentAdd(const ComponentAddedEvent& event);
        void OnComponentRemove(const ComponentRemovedEvent& event);

        ObjectsVecType m_Objects;
        ComponentsVecType m_Components;

        EventBus& m_EventBus;
        EventBusSubscriber m_EventSubscriber;
    };
}
