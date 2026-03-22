#include <LE/Object.hpp>

#include <LE/Application.hpp>

namespace le
{
    Object::Object(const bool calculatesMatrices)
        :
        m_Transform(Matrix4x4f::MakeIdentity()),
        m_CalculatesMatrices(calculatesMatrices)
    {
        CalculateTransformMatrix();
    }

    void Object::Enable()
    {
        m_Enabled = true;
    }

    void Object::Disable()
    {
        m_Enabled = false;
    }

    bool Object::IsEnabled() const
    {
        return m_Enabled;
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

    void Object::ComponentAdded(const std::type_index type,
        Component& component) const
    {
        if (m_pScene)
            m_pScene->OnComponentAdd(type, component);
    }

    void Object::ComponentRemoved(const std::type_index type,
        Component& component) const
    {
        if (m_pScene)
            m_pScene->OnComponentRemove(type, component);
    }

    Object::HolderType& Object::GetComponents()
    {
        return m_Components;
    }

    void Object::ClearComponents()
    {
        for (auto& [key, val] : m_Components)
            ComponentRemoved(key, *val);

        m_Components.clear();
    }
}
