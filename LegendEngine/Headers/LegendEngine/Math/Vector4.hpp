#ifndef _LEGENDENGINE_MATH_VECTOR4_HPP
#define _LEGENDENGINE_MATH_VECTOR4_HPP

#include <LegendEngine/Math/Vector3.hpp>
#include <iostream>

namespace LegendEngine
{
	// Vector4 must be of a numeric type
	template<typename T>
	class Vector4
	{
	public:
		Vector4();
		Vector4(T value);
		Vector4(T x, T y, T z, T w);
		Vector4(Vector2<T> vec);
		Vector4(Vector3<T> vec);

		Vector4(const Vector4& ref);
		Vector4<T> operator=(const Vector4<T>& toSet);

		T& operator[](int index);

		// Arithmetic (+ - * /)
		Vector4<T> operator+(const Vector4<T>& toAdd);
		Vector4<T> operator-(const Vector4<T>& toAdd);
		Vector4<T> operator*(const Vector4<T>& toAdd);
		Vector4<T> operator/(const Vector4<T>& toAdd);
		Vector4<T> operator+(const T& value);
		Vector4<T> operator-(const T& value);
		Vector4<T> operator*(const T& value);
		Vector4<T> operator/(const T& value);

		// Assignment (+= -= *= /=)
		Vector4<T>& operator+=(const Vector4<T>& toAdd);
		Vector4<T>& operator-=(const Vector4<T>& toAdd);
		Vector4<T>& operator*=(const Vector4<T>& toAdd);
		Vector4<T>& operator/=(const Vector4<T>& toAdd);
		Vector4<T>& operator+=(const T& value);
		Vector4<T>& operator-=(const T& value);
		Vector4<T>& operator*=(const T& value);
		Vector4<T>& operator/=(const T& value);

		T x = 0.0f;
		T y = 0.0f;
		T z = 0.0f;
		T w = 0.0f;

		static Vector4<T> Normalize(Vector4<T> v);
		static float Dot(Vector4<T> v1, Vector4<T> v2);
		static float Length(Vector4<T> v);
	};
}

#include <LegendEngine/Math/Vector4.inl>

#endif //_LEGENDENGINE_MATH_VECTOR4_HPP