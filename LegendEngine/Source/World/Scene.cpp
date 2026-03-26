#include <ranges>
#include <LE/Application.hpp>
#include <LE/World/Scene.hpp>
#include <LE/World/Entity.hpp>

namespace le
{
    Entity Scene::CreateEntity()
    {
        const Entity obj(*this);
        m_entities[obj.uid] = {};

        LE_DEBUG("Added object with uid {} to scene {:#x}",
            static_cast<uint64_t>(obj.uid), reinterpret_cast<size_t>(this));

        return obj;
    }

    bool Scene::HasEntity(const Entity& entity) const
    {
        return HasEntity(entity.uid);
    }

    void Scene::DeleteEntity(const Entity& entity)
    {
        return DeleteEntity(entity.uid);
    }

    bool Scene::HasEntity(const UID entity) const
    {
        return m_entities.contains(entity);
    }

    void Scene::DeleteEntity(const UID entity)
    {
        auto [archetypeID, row] = m_entities.at(entity);
        m_entities.erase(entity);

        // If an entity has no archetype, it has no components, so there is nothing to delete
        if (!m_archetypes.contains(archetypeID))
            return;

        Archetype& archetype = m_archetypes[archetypeID];

        // Swap and pop the entity ID
        archetype.entityIDs[row] = archetype.entityIDs.back();
        archetype.entityIDs.pop_back();

        // Pop all the components
        for (ComponentStorage& storage : archetype.componentData | std::views::values)
            storage.SwapAndPop(row);

        ClearCachedArchetypeLookups();
    }

    void Scene::Clear()
    {
        m_entities.clear();
        m_findArchetypeResults.clear();
        m_archetypes.clear();
    }

    void Scene::ClearCachedArchetypeLookups()
    {
        m_findArchetypeResults.clear();
    }
}
