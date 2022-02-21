// A base object class to then be inherited by other types of objects.

#ifndef _LEGENDENGINE_OBJECT3D_HPP
#define _LEGENDENGINE_OBJECT3D_HPP

#include <LegendEngine/Common/Math.hpp>
#include <LegendEngine/Object3d/Components/ComponentHolder.hpp>

#include <type_traits>
#include <string>
#include <string_view>

namespace LegendEngine
{
    class Application3D;
}

namespace LegendEngine::Object3d
{
    class Object : public Components::ComponentHolder
    {
        friend Application3D;
    public:
        Object() 
            :
            scale(1.0f),
            Components::ComponentHolder(this)
        {}
        
        Object(const Object&) = delete;
		Object(Object&&) = delete;
		Object& operator=(const Object&) = delete;
		Object& operator=(Object&&) = delete;

        void AddPosition(Vector3f position);
        void AddRotation(Vector3f rotation);
        void AddScale(Vector3f scale);
        void SetPosition(Vector3f position);
        void SetRotation(Vector3f rotation);
        void SetScale(Vector3f scale);
        Vector3f GetPosition();
        Vector3f GetRotation();
        Vector3f GetScale();

        Application3D* GetApplication();
    protected:
        Application3D* pApplication = nullptr;
    private:
        void CalculateTransformMatrix();

        Vector3f position;
        Vector3f rotation;
        Vector3f scale;
    };
}

#endif //_LEGENDENGINE_OBJECT3D_HPP