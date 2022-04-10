#pragma once

#include <memory>

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T>
using Scope = std::unique_ptr<T>;

namespace RefTools
{
    template<class T, class... Args>
    Ref<T> Create(Args&&... args)
    {
        return std::make_shared<T>(args...);
    }
}