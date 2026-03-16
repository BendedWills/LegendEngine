#include <ranges>
#include <LegendEngine/Application.hpp>
#include <LegendEngine/Scene.hpp>

namespace le
{
    bool Scene::HasObject(const Object& object)
    {
        return HasObject(object.uid);
    }

    bool Scene::HasObject(const UID objectID)
    {
        std::shared_lock lock(m_ObjectsMutex);

        if (!m_Objects.contains(objectID))
            return false;

        return m_Objects.at(objectID) != nullptr;
    }

    void Scene::RemoveObject(Object& toRemove)
    {
        std::unique_lock lock(m_ObjectsMutex);
        for (const auto& uid : m_Objects | std::views::keys)
        {
            if (uid != toRemove.uid)
                continue;

            LE_DEBUG("Removed object with uid {} from scene {:#x}",
                     static_cast<uint64_t>(uid), reinterpret_cast<size_t>(this));

            RemoveObjectComponents(toRemove);
            m_Objects.erase(uid);
            return;
        }
    }

    void Scene::Clear()
    {
        m_Objects.clear();
    }

    const Scene::ObjectsType& Scene::GetObjects() const
    {
        return m_Objects;
    }

    Scene::ComponentsVecType Scene::GetObjectComponents()
    {
        std::shared_lock lock(m_ComponentsMutex);
        return m_Components;
    }

    Scope<Scene> Scene::Create()
    {
        return std::make_unique<Scene>();
    }

    void Scene::OnComponentAdd(const std::type_index id, Component& component)
    {
        std::unique_lock lock(m_ComponentsMutex);
        m_Components[id].push_back(&component);
    }

    void Scene::OnComponentRemove(const std::type_index id, Component& component)
    {
        std::unique_lock lock(m_ComponentsMutex);
        std::erase(m_Components[id], &component);
    }

    Object* Scene::AddObject(Scope<Object> object)
    {
        std::unique_lock lock(m_ObjectsMutex);

        const Scope<Object>& added = m_Objects[object->uid] = std::move(object);
        added->m_pScene = this;

        AddObjectComponents(*added);

        LE_DEBUG("Added object with uid {} to scene {:#x}",
                 static_cast<uint64_t>(added->uid), reinterpret_cast<size_t>(this));

        return added.get();
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
}