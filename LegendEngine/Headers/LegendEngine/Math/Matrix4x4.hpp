#ifndef _LEGENDENGINE_MATH_MATRIX4X4_HPP
#define _LEGENDENGINE_MATH_MATRIX4X4_HPP

#include <LegendEngine/Math/Vector4.hpp>
#include <LegendEngine/Math/Quaternion.hpp>

#include <cmath>

namespace LegendEngine
{
	// Matrix4x4 must be of a numeric type
	template<typename T>
	class Matrix4x4
	{
	public:
		static constexpr const uint64_t COL_COUNT = 4;
		
		Matrix4x4();
		Matrix4x4(T value);
		Matrix4x4(Quaternion q);
		
		Matrix4x4(const Matrix4x4& ref);
		Matrix4x4<T> operator=(const Matrix4x4<T>& toSet);

		Vector4<T>& operator[](int index);
		
		// Arithmetic (+ - * /)
		Matrix4x4<T> operator*(const Matrix4x4<T>& m);
		Matrix4x4<T>& operator*(T& m);
		
		// Assignment (+= -= *= /=)
		Matrix4x4<T>& operator*=(const Matrix4x4<T>& m);
		Matrix4x4<T>& operator*=(T m);

		static Matrix4x4<T> MakeIdentity();
		
		Vector4<T> c[COL_COUNT];
	private:
		Matrix4x4<T> Multiply(Matrix4x4<T> m);
	};
}

#include <LegendEngine/Math/Matrix4x4.inl>

#endif //_LEGENDENGINE_MATH_MATRIX4X4_HPP