#include <LegendEngine/Components/ComponentHolder.hpp>

namespace LegendEngine::Components
{
    ComponentHolder::~ComponentHolder()
    {}

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
            SpawnRemoveEvent(key, *val);

        m_Components.clear();
    }
}