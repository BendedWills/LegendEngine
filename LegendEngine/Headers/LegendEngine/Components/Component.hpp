#pragma once

#include <LegendEngine/Common/Defs.hpp>

namespace LegendEngine::Objects
{
    class Object;
}

namespace LegendEngine::Components
{
    class Component
    {
        friend class ComponentHolder;
    public:
        Component() = default;
        virtual ~Component() = default;
        LEGENDENGINE_NO_COPY(Component);

        Objects::Object& GetObject() const;
    protected:
        Objects::Object* m_pObject = nullptr;
    };
}