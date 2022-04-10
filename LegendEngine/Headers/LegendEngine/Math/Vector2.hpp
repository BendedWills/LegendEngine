#ifndef _LEGENDENGINE_MATH_VECTOR2_HPP
#define _LEGENDENGINE_MATH_VECTOR2_HPP

#include <iostream>
#include <cmath>

namespace LegendEngine
{
	// Vector2 must be of a numeric type
	template<typename T>
	class Vector2
	{
	public:
		Vector2();
		Vector2(T value);
		Vector2(T x, T y);

		Vector2(const Vector2& ref);
		Vector2<T> operator=(const Vector2<T>& toSet);

		T& operator[](int index);

		// Arithmetic (+ - * /)
		Vector2<T> operator+(const Vector2<T>& value);
		Vector2<T> operator-(const Vector2<T>& value);
		Vector2<T> operator*(const Vector2<T>& value);
		Vector2<T> operator/(const Vector2<T>& value);
		Vector2<T> operator+(const T& value);
		Vector2<T> operator-(const T& value);
		Vector2<T> operator*(const T& value);
		Vector2<T> operator/(const T& value);

		// Assignment (+= -= *= /=)
		Vector2<T>& operator+=(const Vector2<T>& value);
		Vector2<T>& operator-=(const Vector2<T>& value);
		Vector2<T>& operator*=(const Vector2<T>& value);
		Vector2<T>& operator/=(const Vector2<T>& value);
		Vector2<T>& operator+=(const T& value);
		Vector2<T>& operator-=(const T& value);
		Vector2<T>& operator*=(const T& value);
		Vector2<T>& operator/=(const T& value);

		T x = 0.0f;
		T y = 0.0f;

		static Vector2<T> Normalize(Vector2<T> v);
		static float Dot(Vector2<T> v1, Vector2<T> v2);
		static float Length(Vector2<T> v);
	};
}

#include <LegendEngine/Math/Vector2.inl>

#endif //_LEGENDENGINE_MATH_VECTOR2_HPP