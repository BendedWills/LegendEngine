// A base object class to then be inherited by other types of objects.

#ifndef _LEGENDENGINE_OBJECT_HPP
#define _LEGENDENGINE_OBJECT_HPP

#include <LegendEngine/Math/Math.hpp>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Common/Ref.hpp>
#include <LegendEngine/Common/NativeHolder.hpp>
#include <LegendEngine/Objects/Components/ComponentHolder.hpp>
#include <LegendEngine/Objects/Scripts/ScriptHolder.hpp>

#include <string>
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
        friend Object;
    public:
        LEGENDENGINE_NO_COPY(IObjectNative);

        IObjectNative(Object* pObject)
            :
            pObject(pObject)
        {}

		struct ObjectUniforms
		{
			Matrix4x4f transform;
            // Material material;
		};

        void UpdateUniforms();
    protected:
		virtual void OnUniformsUpdate() {}
		virtual void OnDispose() {}

        Object* pObject = nullptr;
    };

    class Object : 
        public Components::ComponentHolder, 
        public Scripts::ScriptHolder, 
        public NativeHolder<IObjectNative>,
        public IDisposable
    {
        friend Application;
        friend Scene;
        friend IObjectNative;
        friend LegendEngine::IRenderer;
    public:
        LEGENDENGINE_DISPOSE_ON_DESTRUCT(Object);
        LEGENDENGINE_NO_COPY(Object);
        
        Object();

        void AddPosition(Vector3f position);
        void AddRotation(Vector3f rotation);
        void AddScale(Vector3f scale);
        void SetPosition(Vector3f position);
        void SetRotation(Vector3f rotation);
        void SetScale(Vector3f scale);
        Vector3f GetPosition();
        Vector3f GetRotation();
        Vector3f GetScale();

        void SetEnabled(bool enabled);
        bool IsEnabled();

        Matrix4x4f& GetTransformationMatrix();

        Application* GetApplication();
    protected:
        void AddToScene(Scene* pScene);
        void RemoveFromScene(Scene* pScene);

        virtual void OnPositionChange() {}
        virtual void OnRotationChange() {}
		virtual void OnScaleChange() {}

		Vector3f position;
		Vector3f rotation;
		Vector3f scale;

        Matrix4x4f transform;

        bool objCalculateMatrices = true;
        bool enabled = true;

        bool updateUniforms = false;

        Application* pApplication = nullptr;
    private:
        void OnDispose();

        void OnComponentAdd(const std::string& typeName, 
            Components::Component* pComponent);
        void OnComponentRemove(const std::string& typeName, 
            Components::Component* pComponent);
        
        void CalculateTransformMatrix();
        
        std::vector<Scene*> scenes;
    };
}

#endif //_LEGENDENGINE_OBJECT_HPP