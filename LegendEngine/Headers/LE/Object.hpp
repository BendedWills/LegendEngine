#pragma once

#include <LE/Common/UID.hpp>

namespace le
{
    class Scene;
    class Object final
    {
    public:
        void Enable();
        void Disable();
        [[nodiscard]] bool IsEnabled() const;

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

        UID uid;
    private:
        Scene* m_pScene = nullptr;
    };
}
