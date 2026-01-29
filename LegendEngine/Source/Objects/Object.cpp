#include <LegendEngine/Objects/Object.hpp>

#include <LegendEngine/Application.hpp>

#include <LegendEngine/Events/ComponentAddedEvent.hpp>
#include <LegendEngine/Events/ComponentRemovedEvent.hpp>
#include <LegendEngine/Events/ObjectDestroyedEvent.hpp>

namespace LegendEngine::Objects
{
    Object::Object(const bool calculatesMatrices)
        :
        m_Transform(Matrix4x4f::MakeIdentity()),
        m_CalculatesMatrices(calculatesMatrices)
    {
        SetObject(this);
        CalculateTransformMatrix();

        std::stringstream str;
        str << "Created Object (" << reinterpret_cast<uint64_t>(this) << ")";

        Application::Get().GetLogger().Log(Logger::Level::DEBUG, str.str());
    }

    Object::~Object()
    {
        Application::Get().GetEventBus().DispatchEvent<Events::ObjectDestroyedEvent>(
            Events::ObjectDestroyedEvent(*this));
    }

    void Object::AddPosition(const Vector3f& position)
    {
        m_Position += position;
        m_Dirty = true;
    }

    void Object::AddScale(const Vector3f& scale)
    {
        m_Scale += scale;
        m_Dirty = true;
    }

    void Object::SetPosition(const Vector3f& position)
    {
        m_Position = position;
        m_Dirty = true;
    }

    void Object::SetScale(const Vector3f& scale)
    {
        m_Scale = scale;
        m_Dirty = true;
    }

    Vector3f Object::GetPosition()
    {
        return m_Position;
    }

    Vector3f Object::GetScale()
    {
        return m_Scale;
    }

    void Object::SetRotation(const Vector3f& rotation)
    {
        m_Rotation = Math::Euler(rotation);
        m_Dirty = true;
    }

    void Object::SetRotation(const Quaternion& q)
    {
        m_Rotation = q;
        m_Dirty = true;
    }

    Quaternion Object::GetRotation()
    {
        return m_Rotation;
    }

    bool Object::IsDirty() const
    {
        return m_Dirty;
    }

    Matrix4x4f Object::GetTransformationMatrix() const
    {
        return m_Transform;
    }

    void Object::CalculateTransformMatrix()
    {
        if (!m_CalculatesMatrices)
            return;

        m_Transform = Matrix4x4f::MakeIdentity();
        m_Transform = Math::Translate(m_Transform, m_Position);
        m_Transform = Math::Scale(m_Transform, m_Scale);
        m_Transform *= Matrix4x4f(m_Rotation);

        TransformChanged();
        m_Dirty = false;
    }

    void Object::SpawnAddEvent(const std::type_index type,
        Components::Component& component)
    {
        OnComponentAdd(type, component);
        Application::Get().GetEventBus().DispatchEvent<Events::ComponentAddedEvent>(
            Events::ComponentAddedEvent(*this, component, type));
    }

    void Object::SpawnRemoveEvent(const std::type_index type,
        Components::Component& component)
    {
        OnComponentRemove(type, component);
        Application::Get().GetEventBus().DispatchEvent<Events::ComponentRemovedEvent>(
            Events::ComponentRemovedEvent(*this, component, type));
    }
}