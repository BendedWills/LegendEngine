#include <LegendEngine/Application.hpp>
#include <LegendEngine/Scene.hpp>

#include <LegendEngine/Events/ObjectDestroyedEvent.hpp>

namespace le
{
    Scene::Scene()
        :
        m_EventBus(Application::Get().GetEventBus()),
        m_EventSubscriber(m_EventBus)
    {
        ListenForEvents();
    }

    Scene::Scene(EventBus& eventBus)
        :
        m_EventBus(eventBus),
        m_EventSubscriber(m_EventBus)
    {
        ListenForEvents();
    }

    Scene::~Scene()
    {}

    void Scene::AddObject(Object& object)
    {
        if (HasObject(object))
            return;

        LE_DEBUG("Added object {:#x} to scene {:#x}",
            reinterpret_cast<size_t>(&object), reinterpret_cast<size_t>(this));

        m_Objects.push_back(&object);

        AddObjectComponents(object);
    }

    bool Scene::HasObject(const Object& object)
    {
        return std::ranges::find(m_Objects, &object) != m_Objects.end();
    }

    void Scene::RemoveObject(Object& object)
    {
        LE_DEBUG("Removed object {:#x} from scene {:#x}",
            reinterpret_cast<size_t>(&object), reinterpret_cast<size_t>(this));

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

    Scope<Scene> Scene::Create()
    {
        return std::make_unique<Scene>();
    }

    void Scene::ListenForEvents()
    {
        m_EventBus.Subscribe<ComponentAddedEvent>(m_EventSubscriber,
                                                          [this](const ComponentAddedEvent& event)
                                                          {
                                                              OnComponentAdd(event);
                                                          });

        m_EventBus.Subscribe<ComponentRemovedEvent>(m_EventSubscriber,
                                                            [this](const ComponentRemovedEvent& event)
                                                            {
                                                                OnComponentRemove(event);
                                                            });

        m_EventBus.Subscribe<ObjectDestroyedEvent>(m_EventSubscriber,
                                                           [this](const ObjectDestroyedEvent& event)
                                                           {
                                                               RemoveObject(event.GetObject());
                                                           });
    }

    void Scene::AddObjectComponents(Object& object)
    {
        for (auto& [key, value] : object.GetComponents())
            m_Components[key].push_back(value.get());
    }

    void Scene::RemoveObjectComponents(Object& object)
    {
        for (auto& [key, value] : object.GetComponents())
            std::erase(m_Components[key], value.get());
    }

    void Scene::OnComponentAdd(const ComponentAddedEvent& event)
    {
        if (!HasObject(event.GetObject()))
            return;

        m_Components[event.GetComponentTypeIndex()].push_back(&event.GetComponent());
    }

    void Scene::OnComponentRemove(const ComponentRemovedEvent& event)
    {
        if (!HasObject(event.GetObject()))
            return;

        std::erase(m_Components[event.GetComponentTypeIndex()], &event.GetComponent());
    }
}