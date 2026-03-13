#pragma once

#include <LegendEngine/Objects/Object.hpp>

#include <memory>
#include <type_traits>

namespace le
{
    class ObjectCreator
    {
    public:
        template<typename T, typename... Args>
            requires std::is_base_of_v<Object, T>
        static Scope<T> Create(Args... args)
        {
            Scope<T> object = std::make_unique<T>(args...);

            LE_DEBUG("Object created (&object = {:#x}, typeid().name() = \"{}\")",
                reinterpret_cast<size_t>(object.get()),
                typeid(T).name());

            return object;
        }
    };
}