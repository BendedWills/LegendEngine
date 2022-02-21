#ifndef _LEGENDENGINE_COMPONENT3D_HPP
#define _LEGENDENGINE_COMPONENT3D_HPP

namespace LegendEngine::Object3d
{
    class Object;
}

namespace LegendEngine::Object3d::Components
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

#endif //_LEGENDENGINE_COMPONENT3D_HPP