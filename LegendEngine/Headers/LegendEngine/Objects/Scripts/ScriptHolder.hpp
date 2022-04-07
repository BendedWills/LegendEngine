#ifndef _LEGENDENGINE_SCRIPTHOLDER_HPP
#define _LEGENDENGINE_SCRIPTHOLDER_HPP

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/Ref.hpp>
#include <LegendEngine/Common/TypeTools.hpp>
#include <LegendEngine/Objects/Scripts/Script.hpp>

#include <unordered_map>
#include <string>

namespace LegendEngine::Objects::Scripts
{
    class ScriptHolder
    {
    public:
        ScriptHolder() {}
        ScriptHolder(Object* pObject)
            :
            pObject(pObject)
        {}
        LEGENDENGINE_NO_COPY(ScriptHolder);

        template<typename T, typename... Args>
        T* AddScript(Args... args)
        {
            AssureScript<T>(args...);
            return (T*)scripts[TypeTools::GetTypeName<T>()].get();
        }

        template<class T>
        bool HasScript()
        {
            return scripts.count(TypeTools::GetTypeName<T>()) != 0;
        }

        template<class T>
        T* GetScript()
        {
            std::string id = TypeTools::GetTypeName<T>();
            if (scripts.count(id) != 0)
                return (T*)scripts[id].get();

            return nullptr;
        }

        template<class T>
        bool RemoveScript()
        {
            std::string id = TypeTools::GetTypeName<T>();
            if (scripts.count(id))
            {
                auto scriptIter = scripts.find(id);
                scripts.erase(scriptIter);

                return true;
            }

            return false;
        }

        std::unordered_map<std::string, Ref<Script>>* GetScripts()
        {
            return &scripts;
        }

        void ClearScripts()
        {
            scripts.clear();
        }
    protected:
        // Returns true if the component was created
        template<typename T, typename... Args>
        bool AssureScript(Args... args)
        {
            std::string id = TypeTools::GetTypeName<T>();
            if (scripts.count(id) == 0)
            {
                Ref<Script> script = RefTools::Create<T>(args...);
                scripts[id] = script;

                script->pObject = pObject;
                script->OnInit();

                return true;
            }

            return false;
        }

        std::unordered_map<std::string, Ref<Script>> scripts;
    private:
        Objects::Object* pObject = nullptr;
    };
}

#endif //_LEGENDENGINE_SCRIPTHOLDER_HPP