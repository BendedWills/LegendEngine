#ifndef _LEGENDENGINE_MATH_FUNCS_VECTOR3_HPP
#define _LEGENDENGINE_MATH_FUNCS_VECTOR3_HPP

#include <LegendEngine/Math/Vector3.hpp>

namespace LegendEngine::Math
{
	template<typename T>
	Vector3<T> Normalize(Vector3<T> v);
	template<typename T>
	T Dot(Vector3<T> v1, Vector3<T> v2);
	template<typename T>
	T Length(Vector3<T> v);

	template<typename T>
	Vector3<T> Cross(Vector3<T> v1, Vector3<T> v2);
}

#include <LegendEngine/Math/Funcs_Vector3.inl>

#endif //_LEGENDENGINE_MATH_FUNCS_VECTOR3_HPP
