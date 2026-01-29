#pragma once

#include <LegendEngine/Objects/Object.hpp>
#include <LegendEngine/Events/EventBus.hpp>
#include <LegendEngine/Events/ComponentAddedEvent.hpp>
#include <LegendEngine/Events/ComponentRemovedEvent.hpp>

#include <memory>
#include <typeindex>
#include <vector>

namespace LegendEngine
{
    class Scene final
    {
    public:
        using ObjectsVecType = std::vector<Objects::Object*>;
        using ComponentsVecType = std::unordered_map<std::type_index,
            std::vector<Components::Component*>>;

        Scene();
        explicit Scene(Events::EventBus& eventBus);
        ~Scene();

        LEGENDENGINE_NO_COPY(Scene);

        void AddObject(Objects::Object& object);
        bool HasObject(const Objects::Object& object);
        void RemoveObject(Objects::Object& object);

        void Clear();

        ObjectsVecType& GetObjects();
        ComponentsVecType& GetObjectComponents();

        static Scope<Scene> Create();
    private:
        void ListenForEvents();

        void AddObjectComponents(Objects::Object& object);
        void RemoveObjectComponents(Objects::Object& object);

        void OnComponentAdd(const Events::ComponentAddedEvent& event);
        void OnComponentRemove(const Events::ComponentRemovedEvent& event);

        ObjectsVecType m_Objects;
        ComponentsVecType m_Components;

        Events::EventBus& m_EventBus;
        Events::EventBusSubscriber m_EventSubscriber;
    };
}
