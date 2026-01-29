#include <LegendEngine/ScriptHolder.hpp>

namespace LegendEngine
{
    ScriptHolder::HolderType& ScriptHolder::GetScripts()
    {
        return m_Scripts;
    }

    void ScriptHolder::ClearScripts()
    {
        m_Scripts.clear();
    }
}