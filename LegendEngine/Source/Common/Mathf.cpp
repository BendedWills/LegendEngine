#include <LegendEngine/Common/Mathf.hpp>

using namespace LegendEngine;

float Mathf::Max(float arg0, float arg1)
{
    return arg0 < arg1 ? arg1 : arg0;
}