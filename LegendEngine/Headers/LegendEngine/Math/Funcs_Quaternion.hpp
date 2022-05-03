#ifndef _LEGENDENGINE_MATH_FUNCS_QUATERNION_HPP
#define _LEGENDENGINE_MATH_FUNCS_QUATERNION_HPP

#include <LegendEngine/Math/Quaternion.hpp>

namespace LegendEngine::Math
{
	Quaternion Normalize(Quaternion q);
	float Dot(Quaternion q1, Quaternion q2);
	float Length(Quaternion q);

	Quaternion Conjugate(Quaternion q);
	Quaternion Inverse(Quaternion q);
	template<typename T>
	Quaternion AngleAxis(T angle, Vector3<T> axis);
	template<typename T>
	Quaternion Euler(Vector3<T> angles);

	template<typename T>
	Vector3<T> Rotate(Quaternion q, Vector3<T> axis);
}

#include <LegendEngine/Math/Funcs_Quaternion.inl>

#endif //_LEGENDENGINE_MATH_FUNCS_QUATERNION_HPP
