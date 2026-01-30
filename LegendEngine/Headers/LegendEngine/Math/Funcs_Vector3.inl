#pragma once

namespace LegendEngine
{
	template<typename T>
	LGENG_MATH_FUNC Vector3<T> Math::Normalize(Vector3<T> v)
	{
		float length = Length(v);
		if (length == 0)
			return v;

		Vector3<T> vec = Vector3<T>(
			v.x / length,
			v.y / length,
			v.z / length
		);

		return vec;
	}

	template<typename T>
	LGENG_MATH_FUNC T Math::Dot(Vector3<T> v1, Vector3<T> v2)
	{
		float product = v1.x * v2.x;
		product += v1.y * v2.y;
		product += v1.z * v2.z;
		return product;
	}

	template<typename T>
	LGENG_MATH_FUNC T Math::Length(Vector3<T> v)
	{
		return sqrt(Dot(v, v));
	}

	template<typename T>
	LGENG_MATH_FUNC Vector3<T> Math::Cross(Vector3<T> v1, Vector3<T> v2)
	{
		return Vector3<T>(
			v1.y * v2.z - v1.z * v2.y,
			v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x
		);
	}
}