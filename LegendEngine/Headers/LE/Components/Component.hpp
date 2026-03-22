#pragma once

#include <LE/Object.hpp>
#include <LE/Common/Defs.hpp>

namespace le
{
    class Component
    {
        friend class Object;
    public:
        Component() = default;
        virtual ~Component();
        LE_NO_COPY(Component);

        [[nodiscard]] Object& GetObject() const;
    protected:
        Object* m_pObject = nullptr;
    };
}