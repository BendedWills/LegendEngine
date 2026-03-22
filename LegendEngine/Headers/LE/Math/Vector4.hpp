#pragma once

#include <stdexcept>

import le.math.vector2;
import le.math.vector3;

namespace le
{
	// Vector4 must be of a numeric type
	template<typename T>
	class Vector4
	{
	public:
		Vector4() = default;
		Vector4(T value)
			:
			x(value),
			y(value),
			z(value),
			w(value)
		{}

		Vector4(T x, T y, T z, T w)
			:
			x(x),
			y(y),
			z(z),
			w(w)
		{}

		Vector4(Vector2<T> vec)
		{
			x = vec.x;
			y = vec.y;
		}

		Vector4(Vector3<T> vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
		}

		Vector4(const Vector4& ref)
		{
			x = ref.x;
			y = ref.y;
			z = ref.z;
			w = ref.w;
		}

		Vector4<T>& operator=(const Vector4<T>& toSet)
		{
			x = toSet.x;
			y = toSet.y;
			z = toSet.z;
			w = toSet.w;
			return *this;
		}

		T& operator[](int index)
		{
			switch (index)
			{
				case 0: return x;
				case 1: return y;
				case 2: return z;
				case 3: return w;
				default: throw std::runtime_error("Vector index out of range!");
			}
		}

		// Arithmetic (+ - * /)
		Vector4<T> operator+(const Vector4<T>& value)
		{
			return Vector4<T>(*this) += value;
		}

		Vector4<T> operator-(const Vector4<T>& value)
		{
			return Vector4<T>(*this) -= value;
		}

		Vector4<T> operator*(const Vector4<T>& value)
		{
			return Vector4<T>(*this) *= value;
		}

		Vector4<T> operator/(const Vector4<T>& value)
		{
			return Vector4<T>(*this) /= value;
		}

		Vector4<T> operator+(const T& value)
		{
			return Vector4<T>(*this) += value;
		}

		Vector4<T> operator-(const T& value)
		{
			return Vector4<T>(*this) -= value;
		}

		Vector4<T> operator*(const T& value)
		{
			return Vector4<T>(*this) *= value;
		}

		Vector4<T> operator/(const T& value)
		{
			return Vector4<T>(*this) /= value;
		}

		// Assignment (+= -= *= /=)
		Vector4<T>& operator+=(const Vector4<T>& value)
		{
			x += value.x;
			y += value.y;
			z += value.z;
			w += value.w;
			return *this;
		}

		Vector4<T>& operator-=(const Vector4<T>& value)
		{
			x -= value.x;
			y -= value.y;
			z -= value.z;
			w -= value.w;
			return *this;
		}

		Vector4<T>& operator*=(const Vector4<T>& value)
		{
			x *= value.x;
			y *= value.y;
			z *= value.z;
			w *= value.w;
			return *this;
		}

		Vector4<T>& operator/=(const Vector4<T>& value)
		{
			x /= value.x;
			y /= value.y;
			z /= value.z;
			w /= value.w;
			return *this;
		}

		Vector4<T>& operator+=(const T& value)
		{
			x += value;
			y += value;
			z += value;
			w += value;
			return *this;
		}

		Vector4<T>& operator-=(const T& value)
		{
			x -= value;
			y -= value;
			z -= value;
			w -= value;
			return *this;
		}

		Vector4<T>& operator*=(const T& value)
		{
			x *= value;
			y *= value;
			z *= value;
			w *= value;
			return *this;
		}

		Vector4<T>& operator/=(const T& value)
		{
			x /= value;
			y /= value;
			z /= value;
			w /= value;
			return *this;
		}

		T x = 0.0f;
		T y = 0.0f;
		T z = 0.0f;
		T w = 0.0f;
	};
}
