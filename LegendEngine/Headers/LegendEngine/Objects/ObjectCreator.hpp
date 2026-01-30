#pragma once

#include <LegendEngine/Objects/Object.hpp>

#include <memory>
#include <type_traits>

namespace LegendEngine::Objects
{
    template<typename T, typename... Args>
        requires std::is_base_of_v<Object, T>
    static Scope<T> Create(Args... args)
    {
        Scope<T> object = std::make_unique<T>(args...);

        LGENG_DEBUG_LOGMANY(
            "Object created (&object = ",
            std::hex, std::showbase,
            reinterpret_cast<size_t>(object.get()),
            ", typeid().name() = \"", typeid(T).name(),
            "\")");

        return object;
    }
}