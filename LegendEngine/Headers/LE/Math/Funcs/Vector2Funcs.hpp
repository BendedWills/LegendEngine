#pragma once

#include <LE/Math/Vector2.hpp>

namespace le::Math
{
	template<typename T>
	Vector2<T> Normalize(Vector2<T> v)
	{
		return Vector2<T>::Normalize(v);
	}

	template<typename T>
	T Dot(Vector2<T> v1, Vector2<T> v2)
	{
		return Vector2<T>::Dot(v1, v2);
	}

	template<typename T>
	T Length(Vector2<T> v)
	{
		return Vector2<T>::Length(v);
	}
}
