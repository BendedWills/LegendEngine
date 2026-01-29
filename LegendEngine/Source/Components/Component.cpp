#include <LegendEngine/Components/Component.hpp>

namespace LegendEngine::Components
{
    Objects::Object& Component::GetObject() const
    {
        return *m_pObject;
    }
}