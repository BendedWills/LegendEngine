#include <LegendEngine/Application.hpp>
#include <LegendEngine/Scene.hpp>

#include <LegendEngine/Events/ObjectDestroyedEvent.hpp>

namespace LegendEngine
{
    Scene::Scene()
        :
        m_EventBus(Application::Get().GetEventBus()),
        m_EventSubscriber(m_EventBus)
    {
        ListenForEvents();
    }

    Scene::Scene(Events::EventBus& eventBus)
        :
        m_EventBus(eventBus),
        m_EventSubscriber(m_EventBus)
    {
        ListenForEvents();
    }

    Scene::~Scene()
    {}

    void Scene::AddObject(Objects::Object& object)
    {
        if (HasObject(object))
            return;

        m_Objects.push_back(&object);

        AddObjectComponents(object);
    }

    bool Scene::HasObject(const Objects::Object& object)
    {
        return std::ranges::find(m_Objects, &object) != m_Objects.end();
    }

    void Scene::RemoveObject(Objects::Object& object)
    {
        RemoveObjectComponents(object);
        std::erase(m_Objects, &object);
    }

    void Scene::Clear()
    {
        m_Objects.clear();
    }

    Scene::ObjectsVecType& Scene::GetObjects()
    {
        return m_Objects;
    }

    Scene::ComponentsVecType& Scene::GetObjectComponents()
    {
        return m_Components;
    }

    void Scene::ListenForEvents()
    {
        m_EventBus.Subscribe<Events::ComponentAddedEvent>(m_EventSubscriber,
        [this](const Events::ComponentAddedEvent& event)
        {
            OnComponentAdd(event);
        });

        m_EventBus.Subscribe<Events::ComponentRemovedEvent>(m_EventSubscriber,
        [this](const Events::ComponentRemovedEvent& event)
        {
            OnComponentRemove(event);
        });

        m_EventBus.Subscribe<Events::ObjectDestroyedEvent>(m_EventSubscriber,
        [this](const Events::ObjectDestroyedEvent& event)
        {
            RemoveObject(event.GetObject());
        });
    }

    void Scene::AddObjectComponents(Objects::Object& object)
    {
        for (auto& [key, value] : object.GetComponents())
            m_Components[key].push_back(value.get());
    }

    void Scene::RemoveObjectComponents(Objects::Object& object)
    {
        for (auto& [key, value] : object.GetComponents())
            std::erase(m_Components[key], value.get());
    }

    void Scene::OnComponentAdd(const Events::ComponentAddedEvent& event)
    {
        if (!HasObject(event.GetObject()))
            return;

        m_Components[event.GetComponentTypeIndex()].push_back(&event.GetComponent());
    }

    void Scene::OnComponentRemove(const Events::ComponentRemovedEvent& event)
    {
        if (!HasObject(event.GetObject()))
            return;

        std::erase(m_Components[event.GetComponentTypeIndex()], &event.GetComponent());
    }
}