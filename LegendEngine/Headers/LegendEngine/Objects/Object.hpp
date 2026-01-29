#pragma once

#include <LegendEngine/ScriptHolder.hpp>
#include <LegendEngine/Components/ComponentHolder.hpp>
#include <LegendEngine/Math/Math.hpp>

namespace LegendEngine
{
    class Scene;
}

namespace LegendEngine::Objects
{
    class Object : public ScriptHolder, public Components::ComponentHolder
    {
    public:
        using ScriptsType = std::unordered_map<std::type_index,
            Scope<Script>>;

        explicit Object(bool calculatesMatrices = true);
        ~Object() override;
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
    protected:
        virtual void TransformChanged() {}

        Vector3f m_Position;
        Vector3f m_Scale = Vector3f(1.0f);

        Quaternion m_Rotation;

        Matrix4x4f m_Transform;

        bool m_Dirty = false;
    private:
        void SpawnAddEvent(std::type_index type, Components::Component& component) override;
        void SpawnRemoveEvent(std::type_index type, Components::Component& component) override;

        bool m_Enabled = true;
        bool m_CalculatesMatrices;
    };
}
