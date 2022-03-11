#include <LegendEngine/Common/IDisposable.hpp>
#include <iostream>

using namespace LegendEngine;

IDisposable::~IDisposable()
{
    Dispose();
}

bool IDisposable::IsInitialized() const
{
    return initialized;
}

void IDisposable::Dispose()
{
    if (!initialized)
    {
        std::cout << "Double disposal" << std::endl;
        return;
    }

    std::cout << "Disposal" << std::endl;
    
    OnDispose();

    initialized = false;
}