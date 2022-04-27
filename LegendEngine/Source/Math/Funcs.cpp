#include <LegendEngine/Math/Funcs.hpp>

#include <cmath>

using namespace LegendEngine;

#define LGENG_ABS_IMPL(type) \
type Math::Abs(type arg0) \
{ \
	return arg0 > 0 ? arg0 : -arg0; \
}

#define LGENG_MAX_IMPL(type) \
type Math::Max(type arg0, type arg1) \
{ \
	return arg0 < arg1 ? arg1 : arg0; \
}

LGENG_ABS_IMPL(float);
LGENG_MAX_IMPL(float);

float Math::Radians(float degrees)
{
	return degrees * PI / 180;
}

float Math::Degrees(float radians)
{
	return radians * 180 / PI;
}
