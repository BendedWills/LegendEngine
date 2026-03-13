#pragma once

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Components/Component.hpp>

#include <unordered_map>
#include <typeindex>
#include <memory>

namespace le
{
	class ComponentHolder
	{
	public:
		using HolderType = std::unordered_map<std::type_index,
			Scope<Component>>;

		ComponentHolder() = default;
		virtual ~ComponentHolder();
		LEGENDENGINE_NO_COPY(ComponentHolder);

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			AssureComponent<T>(args...);
			return *static_cast<T*>(m_Components[typeid(T)].get());
		}

		template<typename T>
		bool HasComponent() const
		{
			return m_Components.contains(typeid(T));
		}

		template<typename T>
		T* GetComponent()
		{
			if (const std::type_index index(typeid(T)); m_Components.contains(index))
				return static_cast<T*>(m_Components[index].get());

			return nullptr;
		}

		template<typename T>
		bool RemoveComponent()
		{
			const std::type_index index(typeid(T));
			const auto componentIter = m_Components.find(index);
			if (componentIter == m_Components.end())
				return false;

			ComponentRemoved(index, *componentIter->second.get());
			m_Components.erase(componentIter);

			return true;
		}

		HolderType& GetComponents();
		void ClearComponents();
	protected:
	    void SetObject(Object* pObject);

		// Returns true if the component was created
		template<typename T, typename... Args>
		bool AssureComponent(Args&&... args)
		{
			const std::type_index index(typeid(T));
			if (m_Components.contains(index))
				return false;

			m_Components.emplace(index, std::make_unique<T>(args...));
		    m_Components[index]->m_pObject = m_pObject;
			ComponentAdded(index, *m_Components[index].get());

			return true;
		}

		virtual void OnComponentAdd(const std::type_index type,
			Component& component)
		{}
		virtual void OnComponentRemove(const std::type_index type,
			Component& component)
		{}

		HolderType m_Components;
	private:
		virtual void ComponentAdded(const std::type_index type, Component& component) = 0;
		virtual void ComponentRemoved(const std::type_index type, Component& component) = 0;

	    Object* m_pObject = nullptr;
	};
}
