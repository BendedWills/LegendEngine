#include <LegendEngine/Components/Component.hpp>

namespace le
{
    Component::~Component()
    {

    }

    Object& Component::GetObject() const
    {
        return *m_pObject;
    }
}