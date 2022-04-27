#ifndef _LEGENDENGINE_MATH_FUNCS_HPP
#define _LEGENDENGINE_MATH_FUNCS_HPP

#define LGENG_MATH_ABS(type) type Abs(type arg0)
#define LGENG_MATH_MAX(type) type Max(type arg0, type arg1)

namespace LegendEngine::Math
{
	constexpr const float PI = 3.14159265358979323846f;
	
	LGENG_MATH_ABS(float);
	LGENG_MATH_MAX(float);
	
	float Radians(float degrees);
	float Degrees(float radians);
}

#endif //_LEGENDENGINE_MATH_FUNCS_HPP