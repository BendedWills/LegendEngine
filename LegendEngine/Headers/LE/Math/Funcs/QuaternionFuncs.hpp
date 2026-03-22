#pragma once

#include <cmath>

import le.math.quaternion;

namespace le::Math
{
	Quaternion Normalize(const Quaternion& q)
	{
		float length = Length(q);

		// Check for divide by 0
		if (length == 0)
			return q;

		return {
			q.w / length,
			q.x / length,
			q.y / length,
			q.z / length
		};
	}

	float Dot(const Quaternion& q1, const Quaternion& q2)
	{
		float product = q1.w * q2.w;
		product += q1.x * q2.x;
		product += q1.y * q2.y;
		product += q1.z * q2.z;
		return product;
	}

	float Length(const Quaternion& q)
	{
		return std::sqrt(Dot(q, q));
	}

	Quaternion Conjugate(const Quaternion& q)
	{
		return { q.w, -q.x, -q.y, -q.z };
	}

	Quaternion Inverse(const Quaternion& q)
	{
		return Conjugate(q) / Dot(q, q);
	}

	template<typename T>
	Quaternion AngleAxis(T angle, Vector3<T> axis)
	{
		T halfAngle = angle / T(2);
		Vector3<T> saxis = axis * std::sin(halfAngle);

		return Quaternion(std::cos(halfAngle), saxis.x, saxis.y, saxis.z);
	}

	template<typename T>
	Quaternion Euler(Vector3<T> vec)
	{
		Vector3<T> halfVec(vec);
		halfVec /= T(2);

		const float cx = std::cos(halfVec.x);
		const float cy = std::cos(halfVec.y);
		const float cz = std::cos(halfVec.z);

		const float sx = std::sin(halfVec.x);
		const float sy = std::sin(halfVec.y);
		const float sz = std::sin(halfVec.z);

		Quaternion q;
		q.w = cx * cy * cz + sx * sy * sz;
		q.x = sx * cy * cz - cx * sy * sz;
		q.y = cx * sy * cz + sx * cy * sz;
		q.z = cx * cy * sz - sx * sy * cz;

		return q;
	}

	template<typename T>
	Vector3<T> Rotate(Quaternion q, Vector3<T> axis)
	{
		return q * axis;
	}
}
