#ifndef _LEGENDENGINE_MATH_FUNCS_VECTOR2_HPP
#define _LEGENDENGINE_MATH_FUNCS_VECTOR2_HPP

#include <LegendEngine/Math/Vector2.hpp>

namespace LegendEngine::Math
{
	template<typename T>
	Vector2<T> Normalize(Vector2<T> v);
	template<typename T>
	T Dot(Vector2<T> v1, Vector2<T> v2);
	template<typename T>
	T Length(Vector2<T> v);
}

#include <LegendEngine/Math/Funcs_Vector2.inl>

#endif //_LEGENDENGINE_MATH_FUNCS_VECTOR2_HPP
