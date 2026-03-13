#include <LegendEngine/Application.hpp>
#include <LegendEngine/Scene.hpp>

#include <LegendEngine/Events/ObjectDestroyedEvent.hpp>

namespace le
{
    Scene::Scene()
        :
        m_EventSubscriber(Application::Get().GetEventBus())
    {
        ListenForEvents();
    }

    Scene::Scene(EventBus& eventBus)
        :
        m_EventSubscriber(eventBus)
    {
        ListenForEvents();
    }

    bool Scene::HasObject(const Object& object) const
    {
        return HasObject(object.uid);
    }

    bool Scene::HasObject(const UID objectID) const
    {
        for (const auto& object : m_Objects)
            if (object && object->uid == objectID)
                return true;

        return false;
    }

    void Scene::RemoveObject(Object& toRemove)
    {
        for (size_t i = 0; i < m_Objects.size(); ++i)
        {
            if (m_Objects[i]->uid != toRemove.uid)
                continue;

            LE_DEBUG("Removed object with uid {} from scene {:#x}",
                 static_cast<uint64_t>(toRemove.uid), reinterpret_cast<size_t>(this));

            RemoveObjectComponents(toRemove);
            m_Objects.erase(m_Objects.begin() + i);
            return;
        }
    }

    void Scene::Clear()
    {
        m_Objects.clear();
    }

    const std::vector<Scope<Object>>& Scene::GetObjects() const
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

    Object* Scene::AddObject(Scope<Object> object)
    {
        // Make sure there are no duplicate UIDs
        // This is like a 1 in a billion chance, but it's technically possible
        while (HasObject(object->uid))
            object->uid = UID();

        const Scope<Object>& added = m_Objects.emplace_back(std::move(object));

        AddObjectComponents(*added);

        LE_DEBUG("Added object with uid {} to scene {:#x}",
            static_cast<uint64_t>(added->uid), reinterpret_cast<size_t>(this));

        return added.get();
    }

    void Scene::ListenForEvents()
    {
        m_EventSubscriber.AddEventHandler<ComponentAddedEvent>(
        [this](const ComponentAddedEvent& event){ OnComponentAdd(event); });
        m_EventSubscriber.AddEventHandler<ComponentRemovedEvent>(
        [this](const ComponentRemovedEvent& event){ OnComponentRemove(event); });
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