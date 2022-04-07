#ifndef _LEGENDENGINE_MATH_FUNCS_HPP
#define _LEGENDENGINE_MATH_FUNCS_HPP

namespace LegendEngine::Math
{
	constexpr const float PI = 3.14159265358979323846;

	float Max(float arg0, float arg1);
	float Radians(float degrees);
	float Degrees(float radians);
	float FastInverseSqrt(float num);
}

#endif //_LEGENDENGINE_MATH_FUNCS_HPP