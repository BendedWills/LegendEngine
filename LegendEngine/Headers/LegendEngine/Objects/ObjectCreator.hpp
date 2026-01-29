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
        return std::make_unique<T>(args...);
    }
}