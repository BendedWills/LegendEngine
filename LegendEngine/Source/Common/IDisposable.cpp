#include <LegendEngine/Common/IDisposable.hpp>

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
    initialized = false;
    
    OnDispose();
}