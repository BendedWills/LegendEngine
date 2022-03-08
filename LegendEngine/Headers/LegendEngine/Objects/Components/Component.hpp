#ifndef _LEGENDENGINE_COMPONENT_HPP
#define _LEGENDENGINE_COMPONENT_HPP

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
        
        Component(const Component&) = delete;
		Component(Component&&) = delete;
		Component& operator=(const Component&) = delete;
		Component& operator=(Component&&) = delete;

        Object* GetObject();
    protected:
        void SetObject(Object* pObject);
    private:
        Object* pObject = nullptr;
    };
}

#endif //_LEGENDENGINE_COMPONENTS_HPP