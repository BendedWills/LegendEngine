namespace LegendEngine
{
	template<typename T>
	LGENG_MATH_FUNC Vector2<T> Math::Normalize(Vector2<T> v)
	{
		// Normalizing a vector is simply dividing it by its length so that
		// its length is one.
		float length = Length(v);
		if (length == 0)
			return v;

		return Vector2<T>(
			v.x / length,
			v.y / length
		);
	}

	template<typename T>
	LGENG_MATH_FUNC T Math::Dot(Vector2<T> v1, Vector2<T> v2)
	{
		float product = v1.x * v2.x;
		product += v1.y * v2.y;
		return product;
	}

	template<typename T>
	LGENG_MATH_FUNC T Math::Length(Vector2<T> v)
	{
		return sqrt(Dot(v, v));
	}
}