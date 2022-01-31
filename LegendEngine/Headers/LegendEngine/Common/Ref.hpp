#pragma once

#include <memory>

template<typename T>
using Ref = std::shared_ptr<T>;

namespace RefTools
{
    template<class T, class... Args>
    std::shared_ptr<T> Create(Args&&... args)
    {
        return std::make_shared<T>(args...);
    }
}