#ifndef _LEGENDENGINE_MATH_VECTOR3_HPP
#define _LEGENDENGINE_MATH_VECTOR3_HPP

#include <LegendEngine/Math/Vector2.hpp>

namespace LegendEngine
{
	// Vector3 must be of a numeric type
	template<typename T>
	class Vector3
	{
	public:
		Vector3();
		Vector3(T value);
		Vector3(T x, T y, T z);
		Vector3(Vector2<T> vec);

		Vector3(const Vector3& ref);
		Vector3<T> operator=(const Vector3<T>& toSet);

		T& operator[](int index);

		// Arithmetic (+ - * /)
		Vector3<T> operator+(const Vector3<T>& toAdd);
		Vector3<T> operator-(const Vector3<T>& toAdd);
		Vector3<T> operator*(const Vector3<T>& toAdd);
		Vector3<T> operator/(const Vector3<T>& toAdd);
		Vector3<T> operator+(const T& value);
		Vector3<T> operator-(const T& value);
		Vector3<T> operator*(const T& value);
		Vector3<T> operator/(const T& value);

		// Assignment (+= -= *= /=)
		Vector3<T>& operator+=(const Vector3<T>& toAdd);
		Vector3<T>& operator-=(const Vector3<T>& toAdd);
		Vector3<T>& operator*=(const Vector3<T>& toAdd);
		Vector3<T>& operator/=(const Vector3<T>& toAdd);
		Vector3<T>& operator+=(const T& value);
		Vector3<T>& operator-=(const T& value);
		Vector3<T>& operator*=(const T& value);
		Vector3<T>& operator/=(const T& value);

		T x = 0.0f;
		T y = 0.0f;
		T z = 0.0f;

		static Vector3<T> Normalize(Vector3<T> v);
		static Vector3<T> Cross(Vector3<T> v1, Vector3<T> v2);
		static float Dot(Vector3<T> v1, Vector3<T> v2);
		static float Length(Vector3<T> v);
	};
}

#include <LegendEngine/Math/Vector3.inl>

#endif //_LEGENDENGINE_MATH_VECTOR3_HPP