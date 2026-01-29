#pragma once

#include <stdexcept>

namespace LegendEngine
{
	template<typename T>
	LGENG_MATH_FUNC Vector4<T>::Vector4()
	{}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T>::Vector4(T value)
		:
		x(value),
		y(value),
		z(value),
		w(value)
	{}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T>::Vector4(T x, T y, T z, T w)
		:
		x(x),
		y(y),
		z(z),
		w(w)
	{}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T>::Vector4(Vector2<T> value)
	{
		x = value.x;
		y = value.y;
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T>::Vector4(Vector3<T> value)
	{
		x = value.x;
		y = value.y;
		z = value.z;
	}

	// The ref is the one to copy from (I always forget that)
	template<typename T>
	LGENG_MATH_FUNC Vector4<T>::Vector4(const Vector4<T>& ref)
	{
		x = ref.x;
		y = ref.y;
		z = ref.z;
		w = ref.w;
	}

	template<typename T>
	LGENG_MATH_FUNC T& Vector4<T>::operator[](int index)
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

	template<typename T>
	LGENG_MATH_FUNC Vector4<T> Vector4<T>::operator=(const Vector4<T>& value)
	{
		x = value.x;
		y = value.y;
		z = value.z;
		w = value.w;
		return *this;
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T> Vector4<T>::operator+(const Vector4<T>& value)
	{
		return Vector4<T>(*this) += value;
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T> Vector4<T>::operator-(const Vector4<T>& value)
	{
		return Vector4<T>(*this) -= value;
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T> Vector4<T>::operator*(const Vector4<T>& value)
	{
		return Vector4<T>(*this) *= value;
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T> Vector4<T>::operator/(const Vector4<T>& value)
	{
		return Vector4<T>(*this) /= value;
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T> Vector4<T>::operator+(const T& value)
	{
		return Vector4<T>(*this) += value;
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T> Vector4<T>::operator-(const T& value)
	{
		return Vector4<T>(*this) -= value;
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T> Vector4<T>::operator*(const T& value)
	{
		return Vector4<T>(*this) *= value;
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T> Vector4<T>::operator/(const T& value)
	{
		return Vector4<T>(*this) /= value;
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T>& Vector4<T>::operator+=(const Vector4<T>& value)
	{
		x += value.x;
		y += value.y;
		z += value.z;
		w += value.w;
		return *this;
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T>& Vector4<T>::operator-=(const Vector4<T>& value)
	{
		x -= value.x;
		y -= value.y;
		z -= value.z;
		w -= value.w;
		return *this;
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T>& Vector4<T>::operator*=(const Vector4<T>& value)
	{
		x *= value.x;
		y *= value.y;
		z *= value.z;
		w *= value.w;
		return *this;
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T>& Vector4<T>::operator/=(const Vector4<T>& value)
	{
		x /= value.x;
		y /= value.y;
		z /= value.z;
		w /= value.w;
		return *this;
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T>& Vector4<T>::operator+=(const T& value)
	{
		x += value;
		y += value;
		z += value;
		w += value;
		return *this;
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T>& Vector4<T>::operator-=(const T& value)
	{
		x -= value;
		y -= value;
		z -= value;
		w -= value;
		return *this;
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T>& Vector4<T>::operator*=(const T& value)
	{
		x *= value;
		y *= value;
		z *= value;
		w *= value;
		return *this;
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T>& Vector4<T>::operator/=(const T& value)
	{
		x /= value;
		y /= value;
		z /= value;
		w /= value;
		return *this;
	}
}