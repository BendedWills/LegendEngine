#ifndef _LEGENDENGINE_COMPONENT_HPP
#define _LEGENDENGINE_COMPONENT_HPP

#include <LegendEngine/Common/Defs.hpp>

namespace LegendEngine::Objects
{
    class Object;
}

namespace LegendEngine::Objects::Components
{
    class ComponentHolder;
    class Component
    {
        friend ComponentHolder;
    public:
        Component();

        // Tells the compiler to generate extra code to figure out the type
        // at runtime and delete the object correctly.
        virtual ~Component() {}

        LEGENDENGINE_NO_COPY(Component);
        
        Object* GetObject();
    protected:
        void SetObject(Object* pObject);
    private:
        Object* pObject = nullptr;
    };
}

#endif //_LEGENDENGINE_COMPONENTS_HPP