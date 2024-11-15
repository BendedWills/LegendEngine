#ifndef _LEGENDENGINE_COMPONENTHOLDER_HPP
#define _LEGENDENGINE_COMPONENTHOLDER_HPP

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/Ref.hpp>
#include <LegendEngine/Common/TypeTools.hpp>
#include <LegendEngine/Objects/Components/Component.hpp>

#include <unordered_map>
#include <string>

#if defined(__clang__) || defined(__GNUC__)
#define LEGENDENGINE_PRETTY_FUNCTION __PRETTY_FUNCTION__
#define LEGENDENGINE_PRETTY_FUNCTION_PREFIX '='
#define LEGENDENGINE_PRETTY_FUNCTION_SUFFIX ';'
#elif defined(_MSC_VER)
#define LEGENDENGINE_PRETTY_FUNCTION __FUNCSIG__
#define LEGENDENGINE_PRETTY_FUNCTION_PREFIX '<'
#define LEGENDENGINE_PRETTY_FUNCTION_SUFFIX '>'
#else
#error "Pretty function unsupported"
#endif

namespace LegendEngine::Objects::Components
{
	class ComponentHolder
	{
	public:
		using HolderType = std::unordered_map<std::string, Scope<Component>>;

		ComponentHolder() {}
		ComponentHolder(Object* pObject)
			:
			pObject(pObject)
		{}

		~ComponentHolder()
		{
			ClearComponents();
		}

		LEGENDENGINE_NO_COPY(ComponentHolder);

		template<typename T, typename... Args>
		T* AddComponent(Args... args)
		{
			AssureComponent<T>(args...);
			return (T*)components[TypeTools::GetTypeName<T>()].get();
		}

		template<typename T>
		bool HasComponent()
		{
			return components.count(TypeTools::GetTypeName<T>()) != 0;
		}

		template<typename T>
		T* GetComponent()
		{
			std::string id = TypeTools::GetTypeName<T>();
			if (components.count(id) != 0)
				return (T*)components[id].get();

			return nullptr;
		}

		template<typename T>
		bool RemoveComponent()
		{
			std::string id = TypeTools::GetTypeName<T>();
			if (components.count(id))
			{
				auto componentIter = components.find(id);

				OnComponentRemove(id, componentIter->second.get());
				components.erase(componentIter);

				return true;
			}

			return false;
		}

		HolderType* GetComponents()
		{
			return &components;
		}

		void ClearComponents()
		{
			components.clear();
		}
	protected:
		// Returns true if the component was created
		template<typename T, typename... Args>
		bool AssureComponent(Args... args)
		{
			std::string id = TypeTools::GetTypeName<T>();
			if (components.count(id) == 0)
			{
				components.emplace(id, std::make_unique<T>(args...));
				components[id]->SetObject(pObject);

				OnComponentAdd(id, components[id].get());

				return true;
			}

			return false;
		}

		virtual void OnComponentAdd(const std::string& typeName, 
			Component* pComponent) 
		{}
		virtual void OnComponentRemove(const std::string& typeName, 
			Component* pComponent)
		{}

		HolderType components;
	private:
		Objects::Object* pObject = nullptr;
	};
}

#endif //_LEGENDENGINE_COMPONENTHOLDER_HPP