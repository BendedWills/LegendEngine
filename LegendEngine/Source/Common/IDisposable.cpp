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
    if (!initialized || (singleDispose && disposed))
        return;

    OnDispose();

    initialized = false;
    disposed = false;
}