namespace LegendEngine
{
	template<typename T>
	LGENG_MATH_FUNC T Math::Abs(T arg0)
	{
		return arg0 > 0 ? arg0 : -arg0;
	}

	template<typename T>
	LGENG_MATH_FUNC T Math::Max(T arg0, T arg1)
	{
		return arg0 < arg1 ? arg1 : arg0;
	}

	template<typename T>
	LGENG_MATH_FUNC T Math::Radians(T degrees)
	{
		return degrees * PI / 180;
	}

	template<typename T>
	LGENG_MATH_FUNC T Math::Degrees(T radians)
	{
		return radians * 180 / PI;
	}
}