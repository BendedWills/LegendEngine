#pragma once

#include <LegendEngine/Common/UID.hpp>
#include <LegendEngine/Components/ComponentHolder.hpp>
#include <LegendEngine/Math/Math.hpp>

namespace le
{
    class Scene;
    class Object : public ComponentHolder
    {
        friend class Scene;
    public:
        explicit Object(bool calculatesMatrices = true);
        LEGENDENGINE_NO_COPY(Object);

        void Enable();
        void Disable();

        void AddPosition(const Vector3f& position);
        void AddScale(const Vector3f& scale);
        void SetPosition(const Vector3f& position);
        void SetScale(const Vector3f& scale);
        Vector3f GetPosition();
        Vector3f GetScale();

        void SetRotation(const Vector3f& rotation);
        void SetRotation(const Quaternion& q);
        Quaternion GetRotation();

        bool IsEnabled() const;
        bool IsDirty() const;

        void CalculateTransformMatrix();
        Matrix4x4f GetTransformationMatrix() const;

        UID uid;
    protected:
        virtual void TransformChanged() {}

        Vector3f m_Position;
        Vector3f m_Scale = Vector3f(1.0f);

        Quaternion m_Rotation;

        Matrix4x4f m_Transform;

        bool m_Dirty = false;

        Scene* m_pScene = nullptr;
    private:
        void ComponentAdded(std::type_index type, Component& component) override;
        void ComponentRemoved(std::type_index type, Component& component) override;

        bool m_Enabled = true;
        bool m_CalculatesMatrices;
    };
}
