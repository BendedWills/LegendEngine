#pragma once

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Script.hpp>

#include <unordered_map>
#include <typeindex>
#include <memory>

namespace LegendEngine
{
    class ScriptHolder
    {
    public:
        using HolderType = std::unordered_map<std::type_index,
            Scope<Script>>;

        ScriptHolder() = default;
        virtual ~ScriptHolder() = default;
        LEGENDENGINE_NO_COPY(ScriptHolder);

        template<typename T, typename... Args>
        T& AddScript(Args&&... args)
        {
            AssureScript<T>(args...);
            return *static_cast<T*>(m_Scripts[typeid(T)].get());
        }

        template<class T>
        bool HasScript() const
        {
            return m_Scripts.contains(typeid(T));
        }

        template<class T>
        T* GetScript()
        {
            if (const std::type_index id = typeid(T);
                m_Scripts.contains(id))
                return static_cast<T*>(m_Scripts[id].get());

            return nullptr;
        }

        template<class T>
        bool RemoveScript()
        {
            const std::type_index id = typeid(T);
            if (!m_Scripts.contains(id))
                return false;

            const auto scriptIter = m_Scripts.find(id);
            m_Scripts.erase(scriptIter);

            return true;
        }

        HolderType& GetScripts();
        void ClearScripts();
    protected:
        // Returns true if the component was created
        template<typename T, typename... Args>
        bool AssureScript(Args&&... args)
        {
            const std::type_index id = typeid(T);
            if (m_Scripts.contains(id))
                return false;

            m_Scripts.emplace(id, std::make_unique<T>(args...));

            return true;
        }

        HolderType m_Scripts;
    };
}
