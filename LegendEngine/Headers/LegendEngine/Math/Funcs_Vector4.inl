#pragma once

namespace LegendEngine
{
	template<typename T>
	LGENG_MATH_FUNC Vector4<T> Math::Normalize(Vector4<T> v)
	{
		float length = Length(v);
		if (length == 0)
			return v;

		return Vector4<T>(
			v.x / length,
			v.y / length,
			v.z / length,
			v.w / length
			);
	}

	template<typename T>
	LGENG_MATH_FUNC T Math::Dot(Vector4<T> v1, Vector4<T> v2)
	{
		float product = v1.x * v2.x;
		product += v1.y * v2.y;
		product += v1.z * v2.z;
		product += v1.w * v2.w;
		return product;
	}

	template<typename T>
	LGENG_MATH_FUNC T Math::Length(Vector4<T> v)
	{
		return sqrt(Dot(v, v));
	}
}