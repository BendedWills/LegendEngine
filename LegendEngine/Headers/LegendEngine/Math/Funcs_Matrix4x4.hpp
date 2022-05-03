#ifndef _LEGENDENGINE_MATH_FUNCS_MATRIX4X4_HPP
#define _LEGENDENGINE_MATH_FUNCS_MATRIX4X4_HPP

#include <LegendEngine/Math/Matrix4x4.hpp>

namespace LegendEngine::Math
{
	template<typename T>
		Matrix4x4<T> Scale(Matrix4x4<T> m, Vector3<T> scale);
		/**
		 * @param rotation The rotation in degrees.
		 */
		template<typename T>
		Matrix4x4<T> Rotate(Matrix4x4<T> m, T angle, Vector3<T> rotation);
		template<typename T>
		Matrix4x4<T> Translate(Matrix4x4<T> m, Vector3<T> position);

		/**
		 * @brief Constructs a view matrix based on where the camera is looking.
		 */
		template<typename T>
		Matrix4x4<T> LookAt(Vector3<T> position, Vector3<T> target,
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
		template<typename T>
		Matrix4x4<T> Ortho(T left, T right, T bottom, T top, T near, T far);

		/**
		 * @brief Creates a perspective projection matrix.
		 *	DirectX coordinates (0 and 1)
		 *
		 * @param fov The vertical field of view in radians.
		 * @param aspect The aspect ratio of the window. Width / Height.
		 * @param nearZ Distance between the near plane.
		 * @param farZ Distance between the far plane.
		 */
		template<typename T>
		Matrix4x4<T> PerspectiveRH_ZO(T fov, T aspect, T nearZ, T farZ);
		/**
		 * @brief Creates a perspective projection matrix.
		 *  OpenGL coordinates (-1 and 1)
		 *
		 * @param fov The vertical field of view in radians.
		 * @param aspect The aspect ratio of the window. Width / Height.
		 * @param nearZ Distance between the near plane.
		 * @param farZ Distance between the far plane.
		 */
		template<typename T>
		Matrix4x4<T> PerspectiveRH_NO(T fov, T aspect, T nearZ, T farZ);
}

#include <LegendEngine/Math/Funcs_Matrix4x4.inl>

#endif //_LEGENDENGINE_MATH_FUNCS_MATRIX4X4_HPP
