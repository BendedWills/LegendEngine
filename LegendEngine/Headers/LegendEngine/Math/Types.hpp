#ifndef _LEGENDENGINE_MATH_TYPES_HPP
#define _LEGENDENGINE_MATH_TYPES_HPP

#include <LegendEngine/Math/Matrix4x4.hpp>
#include <LegendEngine/Math/Vector4.hpp>

namespace LegendEngine
{
	// Matrix types
	using Matrix4x4f = Matrix4x4<float>;

	// Vector types
	using Color = Vector4<float>;
	using Vector2f = Vector2<float>;
	using Vector3f = Vector3<float>;
	using Vector4f = Vector4<float>;
}

#endif //_LEGENDENGINE_MATH_TYPES_HPP