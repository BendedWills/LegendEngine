#pragma once

#include <stdexcept>

namespace le
{
	// Vector2 must be of a numeric type
	template<typename T>
	class Vector2
	{
	public:
		Vector2() = default;
		Vector2(T value)
			:
			x(value),
			y(value)
		{}

		Vector2(T x, T y)
			:
			x(x),
			y(y)
		{}

		Vector2(const Vector2& ref)
		{
			x = ref.x;
			y = ref.y;
		}

		T& operator[](const int index)
		{
			switch (index)
			{
				case 0: return x;
				case 1: return y;
				default: throw std::runtime_error("Vector index out of range!");
			}
		}

		// Arithmetic (+ - * /)
		Vector2<T> operator+(const Vector2<T>& value)
		{
			return Vector2<T>(*this) += value;
		}

		Vector2<T> operator-(const Vector2<T>& value)
		{
			return Vector2<T>(*this) -= value;
		}

		Vector2<T> operator*(const Vector2<T>& value)
		{
			return Vector2<T>(*this) *= value;
		}

		Vector2<T> operator/(const Vector2<T>& value)
		{
			return Vector2<T>(*this) /= value;
		}

		Vector2<T> operator+(const T& value)
		{
			return Vector2<T>(*this) += value;
		}

		Vector2<T> operator-(const T& value)
		{
			return Vector2<T>(*this) -= value;
		}

		Vector2<T> operator*(const T& value)
		{
			return Vector2<T>(*this) *= value;
		}

		Vector2<T> operator/(const T& value)
		{
			return Vector2<T>(*this) /= value;
		}

		// Assignment (+= -= *= /=)
		Vector2<T>& operator+=(const Vector2<T>& value)
		{
			x += value.x;
			y += value.y;
			return *this;
		}

		Vector2<T>& operator-=(const Vector2<T>& value)
		{
			x -= value.x;
			y -= value.y;
			return *this;
		}

		Vector2<T>& operator*=(const Vector2<T>& value)
		{
			x *= value.x;
			y *= value.y;
			return *this;
		}

		Vector2<T>& operator/=(const Vector2<T>& value)
		{
			x /= value.x;
			y /= value.y;
			return *this;
		}

		Vector2<T>& operator+=(const T& value)
		{
			x += value;
			y += value;
			return *this;
		}

		Vector2<T>& operator-=(const T& value)
		{
			x -= value;
			y -= value;
			return *this;
		}

		Vector2<T>& operator*=(const T& value)
		{
			x *= value;
			y *= value;
			return *this;
		}

		Vector2<T>& operator/=(const T& value)
		{
			x /= value;
			y /= value;
			return *this;
		}

		T x = 0.0f;
		T y = 0.0f;

		static Vector2<T> Normalize(Vector2<T> v)
		{
			// Normalizing a vector is simply dividing it by its length so that
			// its length is one.
			float length = Length(v);
			if (length == 0)
				return v;

			return Vector2<T>(
				v.x / length,
				v.y / length
			);
		}

		static float Dot(Vector2<T> v1, Vector2<T> v2)
		{
			float product = v1.x * v2.x;
			product += v1.y * v2.y;
			return product;
		}

		static float Length(Vector2<T> v)
		{
			return sqrt(Dot(v, v));
		}
	};
}
