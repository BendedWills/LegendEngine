#include <LegendEngine/Common/Math.hpp>

using namespace LegendEngine;

float Math::Max(float arg0, float arg1)
{
    return arg0 < arg1 ? arg1 : arg0;
}