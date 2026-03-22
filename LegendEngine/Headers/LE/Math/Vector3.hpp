#pragma once

#include <stdexcept>

import le.math.vector2;

namespace le
{
	// Vector3 must be of a numeric type
	template<typename T>
	class Vector3
	{
	public:
		Vector3() = default;
		Vector3(T value)
			:
			x(value),
			y(value),
			z(value)
		{}

		Vector3(T x, T y, T z)
			:
			x(x),
			y(y),
			z(z)
		{}

		Vector3(Vector2<T> vec)
		{
			x = vec.x;
			y = vec.y;
		}

		Vector3(const Vector3& ref)
		{
			x = ref.x;
			y = ref.y;
			z = ref.z;
		}

		T& operator[](const int index)
		{
			switch (index)
			{
				case 0: return x;
				case 1: return y;
				case 2: return z;
				default: throw std::runtime_error("Vector index out of range!");
			}
		}

		// Arithmetic (+ - * /)
		Vector3<T> operator+(const Vector3<T>& value) const
		{
			return Vector3<T>(*this) += value;
		}

		Vector3<T> operator-(const Vector3<T>& value) const
		{
			return Vector3<T>(*this) -= value;
		}

		Vector3<T> operator*(const Vector3<T>& value) const
		{
			return Vector3<T>(*this) *= value;
		}

		Vector3<T> operator/(const Vector3<T>& value) const
		{
			return Vector3<T>(*this) /= value;
		}

		Vector3<T> operator+(const T& value) const
		{
			return Vector3<T>(*this) += value;
		}

		Vector3<T> operator-(const T& value) const
		{
			return Vector3<T>(*this) -= value;
		}

		Vector3<T> operator*(const T& value) const
		{
			return Vector3<T>(*this) *= value;
		}

		Vector3<T> operator/(const T& value) const
		{
			return Vector3<T>(*this) /= value;
		}

		Vector3<T> operator-()
		{
			return Vector3<T>(
				-x,
				-y,
				-z
			);
		}

		// Assignment (+= -= *= /=)
		Vector3<T>& operator+=(const Vector3<T>& value)
		{
			x += value.x;
			y += value.y;
			z += value.z;
			return *this;
		}

		Vector3<T>& operator-=(const Vector3<T>& value)
		{
			x -= value.x;
			y -= value.y;
			z -= value.z;
			return *this;
		}

		Vector3<T>& operator*=(const Vector3<T>& value)
		{
			x *= value.x;
			y *= value.y;
			z *= value.z;
			return *this;
		}

		Vector3<T>& operator/=(const Vector3<T>& value)
		{
			x /= value.x;
			y /= value.y;
			z /= value.z;
			return *this;
		}

		Vector3<T>& operator+=(const T& value)
		{
			x += value;
			y += value;
			z += value;
			return *this;
		}

		Vector3<T>& operator-=(const T& value)
		{
			x -= value;
			y -= value;
			z -= value;
			return *this;
		}

		Vector3<T>& operator*=(const T& value)
		{
			x *= value;
			y *= value;
			z *= value;
			return *this;
		}

		Vector3<T>& operator/=(const T& value)
		{
			x /= value;
			y /= value;
			z /= value;
			return *this;
		}

		T x = 0.0f;
		T y = 0.0f;
		T z = 0.0f;
	};
}
