#include <LE/Components/Transform.hpp>

namespace le
{
    void Transform::AddPosition(const Vector3f& position)
    {
        m_position += position;
        dirty = true;
    }

    void Transform::AddScale(const Vector3f& scale)
    {
        m_scale += scale;
        dirty = true;
    }

    void Transform::SetPosition(const Vector3f& position)
    {
        m_position = position;
        dirty = true;
    }

    void Transform::SetScale(const Vector3f& scale)
    {
        m_scale = scale;
        dirty = true;
    }

    Vector3f Transform::GetPosition() const
    {
        return m_position;
    }

    Vector3f Transform::GetScale()
    {
        return m_scale;
    }

    void Transform::SetRotation(const Vector3f& rotation)
    {
        m_rotation = Math::Euler(rotation);
        dirty = true;
    }

    void Transform::SetRotation(const Quaternion& q)
    {
        m_rotation = q;
        dirty = true;
    }

    Quaternion Transform::GetRotation() const
    {
        return m_rotation;
    }

    void Transform::CalculateTransformMatrix()
    {
        m_transformMat = Matrix4x4f::MakeIdentity();
        m_transformMat = Math::Translate(m_transformMat, m_position);
        m_transformMat = Math::Scale(m_transformMat, m_scale);
        m_transformMat *= Matrix4x4f(m_rotation);

        dirty = false;
    }
}
