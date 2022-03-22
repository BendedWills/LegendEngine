// A base object class to then be inherited by other types of objects.

#ifndef _LEGENDENGINE_OBJECT_HPP
#define _LEGENDENGINE_OBJECT_HPP

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Common/Ref.hpp>
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
    class IRenderer;
}

namespace LegendEngine::Objects
{
    class Object;
    class IObjectNative
    {
    public:
        LEGENDENGINE_NO_COPY(IObjectNative);

        struct ObjectUniforms
        {
            float brightness;
        };

        IObjectNative(Object* pObject)
            :
            pObject(pObject)
        {}

        virtual void OnUniformsUpdate() {}

        virtual void OnDispose() {}
    private:
        Object* pObject = nullptr;
    };

    class Object : public Components::ComponentHolder, IDisposable
    {
        friend Application;
        friend Scene;
        friend IObjectNative;
        friend LegendEngine::IRenderer;
    public:
        LEGENDENGINE_DISPOSE_ON_DESTRUCT(Object);
        LEGENDENGINE_NO_COPY(Object);
        
		Object()
			:
			scale(1.0f),
			Components::ComponentHolder(this)
		{
            initialized = true;
        }

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

        void SetNative(Ref<IObjectNative> native);
        IObjectNative* GetNative();

		Vector3f position;
		Vector3f rotation;
		Vector3f scale;

        Application* pApplication = nullptr;
    private:
        void OnDispose();

        void OnComponentAdd(const std::string& typeName, 
            Components::Component* pComponent);
        void OnComponentRemove(const std::string& typeName, 
            Components::Component* pComponent);
        
        void CalculateTransformMatrix();
        
        bool nativeSet = false;
        Ref<IObjectNative> native;
        
        std::vector<Scene*> scenes;
    };
}

#endif //_LEGENDENGINE_OBJECT3D_HPP