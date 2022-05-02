#ifndef _LEGENDENGINE_MATH_FUNCS_HPP
#define _LEGENDENGINE_MATH_FUNCS_HPP

// Needed for most .inl files too
#include <cmath>

#include <LegendEngine/Math/Constants.hpp>
#include <LegendEngine/Math/Quaternion.hpp>
#include <LegendEngine/Math/Matrix4x4.hpp>
#include <LegendEngine/Math/Vector2.hpp>
#include <LegendEngine/Math/Vector3.hpp>
#include <LegendEngine/Math/Vector4.hpp>

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

	Quaternion Normalize(Quaternion q);
	float Dot(Quaternion q1, Quaternion q2);
	float Length(Quaternion q);

	template<typename T>
	Quaternion AngleAxis(T angle, Vector3<T> axis);
	template<typename T>
	Quaternion Euler(Vector3<T> angles);

// --------- Vector functions ---------

	template<typename T>
	Vector2<T> Normalize(Vector2<T> v);
	template<typename T>
	Vector3<T> Normalize(Vector3<T> v);
	template<typename T>
	Vector4<T> Normalize(Vector4<T> v);

	template<typename T>
	T Dot(Vector2<T> v1, Vector2<T> v2);
	template<typename T>
	T Dot(Vector3<T> v1, Vector3<T> v2);
	template<typename T>
	T Dot(Vector4<T> v1, Vector4<T> v2);

	template<typename T>
	T Length(Vector2<T> v);
	template<typename T>
	T Length(Vector3<T> v);
	template<typename T>
	T Length(Vector4<T> v);

	template<typename T>
	Vector3<T> Cross(Vector3<T> v1, Vector3<T> v2);

// ---------

	template<typename T>
	T Abs(T arg0);
	template<typename T>
	T Max(T arg0, T arg1);

	template<typename T>
	T Radians(T degrees);
	template<typename T>
	T Degrees(T radians);
}

#include <LegendEngine/Math/Funcs.inl>
#include <LegendEngine/Math/Funcs_Matrix4x4.inl>
#include <LegendEngine/Math/Funcs_Quaternion.inl>
#include <LegendEngine/Math/Funcs_Vector2.inl>
#include <LegendEngine/Math/Funcs_Vector3.inl>
#include <LegendEngine/Math/Funcs_Vector4.inl>

#endif //_LEGENDENGINE_MATH_FUNCS_HPP