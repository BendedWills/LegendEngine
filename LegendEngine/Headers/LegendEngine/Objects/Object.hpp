#pragma once

#include <mutex>
#include <shared_mutex>
#include <LegendEngine/Common/UID.hpp>
#include <LegendEngine/Math/Math.hpp>

#include <unordered_map>
#include <typeindex>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Components/Component.hpp>


namespace le
{
    class Scene;
    class Object
    {
        friend class Scene;
    public:
        using HolderType = std::unordered_map<std::type_index,
                                              Scope<Component>>;

        explicit Object(bool calculatesMatrices = true);
        virtual ~Object() = default;
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

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            AssureComponent<T>(args...);
            return *static_cast<T*>(m_Components[typeid(T)].get());
        }

        template<typename T>
        bool HasComponent() const
        {
            std::shared_lock lock(m_ComponentsMutex);
            return m_Components.contains(typeid(T));
        }

        template<typename T>
        T* GetComponent()
        {
            std::shared_lock lock(m_ComponentsMutex);

            if (const std::type_index index(typeid(T)); m_Components.contains(index))
                return static_cast<T*>(m_Components[index].get());

            return nullptr;
        }

        template<typename T>
        bool RemoveComponent()
        {
            std::unique_lock lock(m_ComponentsMutex);

            const std::type_index index(typeid(T));
            const auto componentIter = m_Components.find(index);
            if (componentIter == m_Components.end())
                return false;

            ComponentRemoved(index, *componentIter->second);
            m_Components.erase(componentIter);

            return true;
        }

        HolderType& GetComponents();
        void ClearComponents();

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
        // Returns true if the component was created
        template<typename T, typename... Args>
        bool AssureComponent(Args&&... args)
        {
            std::unique_lock lock(m_ComponentsMutex);

            const std::type_index index(typeid(T));
            if (m_Components.contains(index))
                return false;

            m_Components.emplace(index, std::make_unique<T>(args...));
            m_Components[index]->m_pObject = this;
            ComponentAdded(index, *m_Components[index].get());

            return true;
        }

        void ComponentAdded(std::type_index type, Component& component) const;
        void ComponentRemoved(std::type_index type, Component& component) const;

        bool m_Enabled = true;
        bool m_CalculatesMatrices;

        std::shared_mutex m_ComponentsMutex;
        HolderType m_Components;
    };
}
