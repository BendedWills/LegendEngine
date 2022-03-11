// A base object class to then be inherited by other types of objects.

#ifndef _LEGENDENGINE_OBJECT_HPP
#define _LEGENDENGINE_OBJECT_HPP

#include <LegendEngine/Common/Math.hpp>
#include <LegendEngine/Objects/Components/ComponentHolder.hpp>

#include <type_traits>
#include <string>
#include <string_view>
#include <vector>

namespace LegendEngine
{
    class Application;
    class Scene;
}

namespace LegendEngine::Objects
{
    class Object : public Components::ComponentHolder
    {
        friend Application;
        friend Scene;
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

        Application* GetApplication();
    protected:
        void AddToScene(Scene* pScene);
        void RemoveFromScene(Scene* pScene);

        Application* pApplication = nullptr;
    private:
        void OnComponentAdd(const std::string& typeName, 
            Components::Component* pComponent);
        void OnComponentRemove(const std::string& typeName, 
            Components::Component* pComponent);
        
        void CalculateTransformMatrix();

        Vector3f position;
        Vector3f rotation;
        Vector3f scale;

        std::vector<Scene*> scenes;
    };
}

#endif //_LEGENDENGINE_OBJECT3D_HPP