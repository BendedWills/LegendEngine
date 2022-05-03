#ifndef _LEGENDENGINE_MATH_FUNCS_VECTOR4_HPP
#define _LEGENDENGINE_MATH_FUNCS_VECTOR4_HPP

#include <LegendEngine/Math/Vector4.hpp>

namespace LegendEngine::Math
{
	template<typename T>
	Vector4<T> Normalize(Vector4<T> v);
	template<typename T>
	T Dot(Vector4<T> v1, Vector4<T> v2);
	template<typename T>
	T Length(Vector4<T> v);
}

#include <LegendEngine/Math/Funcs_Vector4.inl>

#endif //_LEGENDENGINE_MATH_FUNCS_VECTOR4_HPP
