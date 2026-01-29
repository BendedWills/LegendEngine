#include <LegendEngine/Components/Component.hpp>

namespace LegendEngine::Components
{
    Component::~Component()
    {

    }

    Objects::Object& Component::GetObject() const
    {
        return *m_pObject;
    }
}