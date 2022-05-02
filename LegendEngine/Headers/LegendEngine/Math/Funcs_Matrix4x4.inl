#include <limits>

namespace LegendEngine
{
	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T> Math::Scale(Matrix4x4<T> m, Vector3<T> scale)
	{
		Matrix4x4<T> product;
		product.c[0] = m.c[0] * scale.x;
		product.c[1] = m.c[1] * scale.y;
		product.c[2] = m.c[2] * scale.z;
		product.c[3] = m.c[3];

		return product;
	}

	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T> Math::Rotate(Matrix4x4<T> m, T a, Vector3<T> rotation)
	{
		T ca = cos(a);
		T sa = sin(a);

		Vector3<T> axis = Vector3<T>::Normalize(rotation);
		Vector3<T> temp(axis * (T(1) - ca));

		Matrix4x4<T> product;
		product[0][0] = ca + temp[0] * axis[0];
		product[0][1] = temp[0] * axis[1] + sa * axis[2];
		product[0][2] = temp[0] * axis[2] - sa * axis[1];

		product[1][0] = temp[1] * axis[0] - sa * axis[2];
		product[1][1] = ca + temp[1] * axis[1];
		product[1][2] = temp[1] * axis[2] + sa * axis[0];

		product[2][0] = temp[2] * axis[0] + sa * axis[1];
		product[2][1] = temp[2] * axis[1] - sa * axis[0];
		product[2][2] = ca + temp[2] * axis[2];

		Matrix4x4<T> result;
		result.c[0] = m.c[0] * product.c[0].x + m.c[1] * product.c[0].y + m.c[2] * product.c[0].z;
		result.c[1] = m.c[0] * product.c[1].x + m.c[1] * product.c[1].y + m.c[2] * product.c[1].z;
		result.c[2] = m.c[0] * product.c[2].x + m.c[1] * product.c[2].y + m.c[2] * product.c[2].z;
		result.c[3] = m.c[3];

		return result;
	}

	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T> Math::Translate(Matrix4x4<T> m, Vector3<T> pos)
	{
		Matrix4x4<T> product = m;
		product.c[3] = m[0] * pos.x + m[1] * pos.y + m[2] * pos.z + m[3];

		return product;
	}

	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T> Math::LookAt(Vector3<T> position, Vector3<T> target,
		Vector3<T> up)
	{
		Vector3<T> centerDir = Vector3<T>::Normalize(target - position);
		Vector3<T> centerUp = Vector3<T>::Normalize(Vector3<T>::Cross(centerDir, up));
		Vector3<T> both = Vector3<T>::Cross(centerUp, centerDir);

		Matrix4x4<T> product = Matrix4x4<T>::MakeIdentity();
		product[0][0] = centerUp.x;
		product[1][0] = centerUp.y;
		product[2][0] = centerUp.z;
		product[0][1] = both.x;
		product[1][1] = both.y;
		product[2][1] = both.z;
		product[0][2] = -centerDir.x;
		product[1][2] = -centerDir.y;
		product[2][2] = -centerDir.z;
		product[3][0] = -Vector3<T>::Dot(centerUp, position);
		product[3][1] = -Vector3<T>::Dot(both, position);
		product[3][2] =  Vector3<T>::Dot(centerDir, position);

		return product;
	}

	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T> Math::Ortho(T left, T right, T bottom, T top, T near, T far)
	{
		Matrix4x4<T> product = Matrix4x4<T>::MakeIdentity();
		product[0][0] =  T(2) / (right - left);
		product[1][1] =  T(2) / (top - bottom);
		product[2][2] = -T(2) / (near - far);
		product[3][0] = -(right + left) / (right - left);
		product[3][1] = -(top + bottom) / (top - bottom);
		product[3][2] = -(far + near)   / (far - near);

		return product;
	}

	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T> Math::PerspectiveRH_ZO(T fov, T aspect, T nearZ, T farZ)
	{
		if (abs(aspect - std::numeric_limits<T>::epsilon()) <= T(0))
			return Matrix4x4<T>(0);

		const T tanHalfFov = tan(fov / static_cast<T>(2));

		Matrix4x4<T> product;
		product[0][0] = T(1) / (aspect * tanHalfFov);
		product[1][1] = T(1) / (tanHalfFov);
		product[2][2] = farZ / (nearZ - farZ);
		product[2][3] = -T(1);
		product[3][2] = -(farZ * nearZ) / (farZ - nearZ);

		return product;
	}

	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T> Math::PerspectiveRH_NO(T fov, T aspect, T nearZ, T farZ)
	{
		if (abs(aspect - std::numeric_limits<T>::epsilon()) <= T(0))
			return Matrix4x4<T>(0);

		const T tanHalfFov = tan(fov / static_cast<T>(2));

		Matrix4x4<T> product;
		product[0][0] = T(1) / (aspect * tanHalfFov);
		product[1][1] = T(1) / (tanHalfFov);
		product[2][2] = -(farZ + nearZ) / (farZ - nearZ);
		product[2][3] = -T(1);
		product[3][2] = -(T(2) * farZ * nearZ) / (farZ - nearZ);

		return product;
	}
}