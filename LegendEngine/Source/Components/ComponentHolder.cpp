#include <LegendEngine/Components/ComponentHolder.hpp>

namespace LegendEngine::Components
{
    ComponentHolder::~ComponentHolder()
    {
        ClearComponents();
    }

    ComponentHolder::HolderType& ComponentHolder::GetComponents()
    {
        return m_Components;
    }

    void ComponentHolder::SetObject(Objects::Object* pObject)
    {
        m_pObject = pObject;
    }

    void ComponentHolder::ClearComponents()
    {
        for (auto& [key, val] : m_Components)
            SpawnRemoveEvent(key, *val.get());

        m_Components.clear();
    }
}