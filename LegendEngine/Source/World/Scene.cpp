#include <ranges>
#include <LE/Application.hpp>
#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/World/Entity.hpp>
#include <LE/World/Scene.hpp>

namespace le
{
#ifndef LE_HEADLESS
    Scene::Scene()
        :
        Scene(Application::Get().GetGraphicsContext(), Application::Get().GetGraphicsResources())
    {}

    Scene::Scene(GraphicsContext& context, const GraphicsResources& resources)
    {
        m_buffer = context.CreatePerFrameBuffer(Buffer::Usage::UNIFORM_BUFFER,
            sizeof(Uniforms));

        DynamicUniforms::DescriptorInfo infos[] =
        {
            {
                DescriptorType::UNIFORM_BUFFER,
                DynamicUniforms::UpdateFrequency::PER_FRAME,
                1,
                &resources.GetSceneLayout()
            },
        };

        m_uniforms = context.CreateDynamicUniforms(std::span(infos));
    }
#endif

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

    void Scene::UpdateUniforms() const
    {
        constexpr Uniforms uniforms;
        m_buffer->Update(sizeof(Uniforms), &uniforms);

        m_uniforms->UpdateBuffer(*m_buffer, 0);
    }
}
