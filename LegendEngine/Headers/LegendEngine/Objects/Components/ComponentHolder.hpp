#ifndef _LEGENDENGINE_COMPONENTHOLDER_HPP
#define _LEGENDENGINE_COMPONENTHOLDER_HPP

#include <LegendEngine/Common/Ref.hpp>
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
        ComponentHolder() {}
        ComponentHolder(Object* pObject)
            :
            pObject(pObject)
        {}

        ComponentHolder(const ComponentHolder&) = delete;
        ComponentHolder(ComponentHolder&&) = delete;
        ComponentHolder& operator=(const ComponentHolder&) = delete;
        ComponentHolder& operator=(ComponentHolder&&) = delete;

        template<typename T, typename... Args>
        T* AddComponent(Args... args)
        {
            AssureComponent<T>(args...);
            return (T*)components[GetTypeName<T>()].get();
        }

        template<typename T>
        bool HasComponent()
        {
            return components.count(GetTypeName<T>()) != 0;
        }

        template<typename T>
        T* GetComponent()
        {
            std::string id = GetTypeName<T>();
            if (components.count(id) != 0)
                return (T*)components[id].get();

            return nullptr;
        }

        template<typename T>
        bool RemoveComponent()
        {
            std::string id = GetTypeName<T>();
            if (components.count(id))
            {
                auto component = components.at(components.find(id));
                components.erase(components.find(id));

                OnComponentRemove(id, component->second.get());

                return true;
            }

            return false;
        }

        std::unordered_map<std::string, Ref<Component>>* GetComponents()
        {
            return &components;
        }

        void ClearComponents()
        {
            components.clear();
        }

        template<typename Type>
        std::string GetTypeName()
        {
            std::string_view prettyFunc{ LEGENDENGINE_PRETTY_FUNCTION };

            auto first = prettyFunc.find_first_not_of(' ',
                prettyFunc.find_first_of(LEGENDENGINE_PRETTY_FUNCTION_PREFIX)
                + 1
            );
            auto last = prettyFunc.find_last_of(
                LEGENDENGINE_PRETTY_FUNCTION_SUFFIX) - first;

            std::string prettyFuncStr(prettyFunc.begin(), prettyFunc.end());

            return prettyFuncStr.substr(first, last);
        }
    protected:
        // Returns true if the component was created
        template<typename T, typename... Args>
        bool AssureComponent(Args... args)
        {
            std::string id = GetTypeName<T>();
            if (components.count(id) == 0)
            {
                components[id] = RefTools::Create<T>(args...);
                components[id]->SetObject(pObject);

                OnComponentAdd(id, components[id].get());

                return true;
            }

            return false;
        }

        virtual void OnComponentAdd(std::string typeName, Component* pComponent) 
        {}
        virtual void OnComponentRemove(std::string typeName, Component* pComponent)
        {}

        std::unordered_map<std::string, Ref<Component>> components;
    private:
        Objects::Object* pObject = nullptr;
    };
}

#endif //_LEGENDENGINE_COMPONENTHOLDER_HPP