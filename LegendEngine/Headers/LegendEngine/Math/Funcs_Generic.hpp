#ifndef _LEGENDENGINE_MATH_FUNCS_GENERIC_HPP
#define _LEGENDENGINE_MATH_FUNCS_GENERIC_HPP

#include <LegendEngine/Math/Matrix4x4.hpp>

namespace LegendEngine::Math
{
	template<typename T>
	T Abs(T arg0);
	template<typename T>
	T Max(T arg0, T arg1);

	template<typename T>
	T Radians(T degrees);
	template<typename T>
	T Degrees(T radians);
}

#include <LegendEngine/Math/Funcs_Generic.inl>

#endif //_LEGENDENGINE_MATH_FUNCS_GENERIC_HPP
