namespace LegendEngine
{
	template<typename T>
	LGENG_MATH_FUNC Quaternion Math::AngleAxis(T angle, Vector3<T> axis)
	{
		T halfAngle = angle / T(2);
		Vector3<T> saxis = axis * std::sin(halfAngle);

		return Quaternion(std::cos(halfAngle), saxis.x, saxis.y, saxis.z);
	}

	template<typename T>
	LGENG_MATH_FUNC Quaternion Math::Euler(Vector3<T> vec)
	{
		Vector3<T> halfVec(vec);
		halfVec /= T(2);

		// Pitch
		float cx = std::cos(halfVec.x);
		float sx = std::sin(halfVec.x);
		// Yaw
		float cy = std::cos(halfVec.y);
		float sy = std::sin(halfVec.y);
		// Roll
		float cz = std::cos(halfVec.z);
		float sz = std::sin(halfVec.z);

		Quaternion q;
		q.w = cx * cy * cz + sx * sy * sz;
		q.x = sx * cy * cz - cx * sy * sz;
		q.y = cx * sy * cz + sx * cy * sz;
		q.z = cx * cy * sz - sx * sy * cz;

		return q;
	}

	LGENG_MATH_FUNC Quaternion Math::Normalize(Quaternion q)
	{
		float length = Length(q);

		// Check for divide by 0
		if (length == 0)
			return q;

		return Quaternion(
			q.x / length,
			q.y / length,
			q.z / length,
			q.w / length
		);
	}

	LGENG_MATH_FUNC float Math::Dot(Quaternion q1, Quaternion q2)
	{
		float product;
		product = q1.w * q2.w;
		product += q1.x * q2.x;
		product += q1.y * q2.y;
		product += q1.z * q2.z;
		return product;
	}

	LGENG_MATH_FUNC float Math::Length(Quaternion q)
	{
		return sqrt(Dot(q, q));
	}
}