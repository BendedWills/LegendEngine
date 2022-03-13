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
        return;

    OnDispose();

    initialized = false;
}