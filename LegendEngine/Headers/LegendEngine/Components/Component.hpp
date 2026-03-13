#pragma once

#include <LegendEngine/Common/Defs.hpp>

namespace le
{
    class Object;
}

namespace le
{
    class Component
    {
        friend class ComponentHolder;
    public:
        Component() = default;
        virtual ~Component();
        LEGENDENGINE_NO_COPY(Component);

        Object& GetObject() const;
    protected:
        Object* m_pObject = nullptr;
    };
}