#include <LegendEngine/Math/Funcs.hpp>

using namespace LegendEngine;

float Math::Max(float arg0, float arg1)
{
    return arg0 < arg1 ? arg1 : arg0;
}

float Math::Radians(float degrees)
{
	return degrees * PI / 180;
}

float Math::Degrees(float radians)
{
	return radians * 180 / PI;
}

float Math::FastInverseSqrt(float num)
{
	const float threehalfs = 1.5F;
	float y = num;

	long i = *(long*)&y;

	i = 0x5f3759df - (i >> 1);
	y = *(float*)&i;

	y = y * (threehalfs - ((num * 0.5F) * y * y));

	return y;
}