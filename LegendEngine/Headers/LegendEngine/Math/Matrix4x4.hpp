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

		static Matrix4x4<T> Scale(Matrix4x4<T> m, Vector3<T> scale);
		/**
		 * @param rotation The rotation in degrees.
		 */
		static Matrix4x4<T> Rotate(Matrix4x4<T> m, T angle, Vector3<T> rotation);
		static Matrix4x4<T> Translate(Matrix4x4<T> m, Vector3<T> position);

		/**
		 * @brief Constructs a view matrix based on where the camera is looking.
		 */
		static Matrix4x4<T> LookAt(Vector3<T> position, Vector3<T> target, 
			Vector3<T> up);

		/**
		 * @brief Creates an orthographic projection matrix.
		 * 
		 * @param left The left coordinate of the frustum.
		 * @param right The right coordinate of the frustum.
		 * @param bottom The bottom coordinate of the frustum.
		 * @param top The top coordinate of the frustum.
		 * @param near Distance between the near plane.
		 * @param far Distance between the far plane.
		 */
		static Matrix4x4<T> Ortho(T left, T right, T bottom, T top, T near, T far);

		/**
		 * @brief Creates a perspective projection matrix.
		 *	DirectX coordinates (0 and 1)
		 *
		 * @param fov The vertical field of view in radians.
		 * @param aspect The aspect ratio of the window. Width / Height.
		 * @param nearZ Distance between the near plane.
		 * @param farZ Distance between the far plane.
		 */
		static Matrix4x4<T> PerspectiveRH_ZO(T fov, T aspect, T nearZ, T farZ);
		/**
		 * @brief Creates a perspective projection matrix. 
		 *  OpenGL coordinates (-1 and 1)
		 *
		 * @param fov The vertical field of view in radians.
		 * @param aspect The aspect ratio of the window. Width / Height.
		 * @param nearZ Distance between the near plane.
		 * @param farZ Distance between the far plane.
		 */
		static Matrix4x4<T> PerspectiveRH_NO(T fov, T aspect, T nearZ, T farZ);

		static Matrix4x4<T> MakeIdentity();
		
		Vector4<T> c[COL_COUNT];
	private:
		Matrix4x4<T> Multiply(Matrix4x4<T> m);
	};
}

#include <LegendEngine/Math/Matrix4x4.inl>
#include <LegendEngine/Math/Matrix4x4Funcs.inl>

#endif //_LEGENDENGINE_MATH_MATRIX4X4_HPP