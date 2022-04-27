#include <LegendEngine/Math/Quaternion.hpp>

using namespace LegendEngine;

Quaternion::Quaternion()
{
	w = 0.7071068f;
	x = 0.7071068f;
}

Quaternion::Quaternion(float w, float x, float y, float z)
	:
	w(w),
	x(x),
	y(y),
	z(z)
{}

Quaternion::Quaternion(const Quaternion& ref)
{
	x = ref.x;
	y = ref.y;
	z = ref.z;
	w = ref.w;
}

Quaternion Quaternion::operator=(const Quaternion& ref)
{
	x = ref.x;
	y = ref.y;
	z = ref.z;
	w = ref.w;
	return *this;
}

Quaternion Quaternion::operator*(const Quaternion& value)
{
	Quaternion product;
	product.x = x * value.x;
	product.y = x * value.y;
	product.z = z * value.z;
	product.w = w * value.w;
	return product;
}

Quaternion& Quaternion::operator*=(const Quaternion& value)
{
	x *= value.x;
	y *= value.y;
	z *= value.z;
	w *= value.w;
	return *this;
}

Vector3<float> Quaternion::GetEulerAngles()
{
	Vector3<float> angles;

	// X
	float sinr_cosp = 2 * (w * x + y * z);
	float cosr_cosp = 1 - 2 * (x * x + y * y);
	angles.x = std::atan2(sinr_cosp, cosr_cosp);

	// Y
	float sinp = 2 * (w * y - z * x);
	if (std::abs(sinp) >= 1)
		angles.y = std::copysign(Math::PI / 2, sinp);
	else
		angles.y = std::asin(sinp);

	// Z
	float siny_cosp = 2 * (w * z + x * y);
	float cosy_cosp = 1 - 2 * (y * y + z * z);
	angles.z = std::atan2(siny_cosp, cosy_cosp);

	return angles;
}

Quaternion Quaternion::Normalize(Quaternion q)
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

float Quaternion::Dot(Quaternion q1, Quaternion q2)
{
	float product;
	product = q1.w * q2.w;
	product += q1.x * q2.x;
	product += q1.y * q2.y;
	product += q1.z * q2.z;
	return product;
}

float Quaternion::Length(Quaternion q)
{
	return sqrt(Dot(q, q));
}

template<typename T>
Quaternion AngleAxis(float angle, Vector3<T> axis)
{
	// TODO
	return *this;
}
