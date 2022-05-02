#include <LegendEngine/Math/Constants.hpp>

namespace LegendEngine
{
	LGENG_MATH_FUNC Quaternion::Quaternion()
	{}

	LGENG_MATH_FUNC Quaternion::Quaternion(float w, float x, float y, float z)
		:
		w(w),
		x(x),
		y(y),
		z(z)
	{}

	LGENG_MATH_FUNC Quaternion::Quaternion(const Quaternion& ref)
	{
		w = ref.w;
		x = ref.x;
		y = ref.y;
		z = ref.z;
	}

	LGENG_MATH_FUNC Quaternion Quaternion::operator=(const Quaternion& ref)
	{
		w = ref.w;
		x = ref.x;
		y = ref.y;
		z = ref.z;
		return *this;
	}

	LGENG_MATH_FUNC Quaternion Quaternion::operator*(const Quaternion& q)
	{
		return Quaternion(*this) *= q;
	}

	LGENG_MATH_FUNC Quaternion& Quaternion::operator*=(const Quaternion& q)
	{
		w = w * q.w - x * q.x - y * q.y - z * q.z;
		x = w * q.x + x * q.w + y * q.z - z * q.y;
		y = w * q.y + y * q.w + z * q.x - x * q.z;
		z = w * q.z + z * q.w + x * q.y - y * q.x;
		return *this;
	}

	LGENG_MATH_FUNC Vector3<float> Quaternion::GetEulerAngles()
	{
		Vector3<float> angles;

		// X
		float sinr_cosp = 2.0f * (w * x + y * z);
		float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
		angles.x = std::atan2(sinr_cosp, cosr_cosp);

		// Y
		float sinp = 2.0f * (w * y - z * x);
		if (std::abs(sinp) >= 1)
			angles.y = std::copysign(Math::PI / 2.0f, sinp);
		else
			angles.y = std::asin(sinp);

		// Z
		float siny_cosp = 2.0f * (w * z + x * y);
		float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
		angles.z = std::atan2(siny_cosp, cosy_cosp);

		return angles;
	}
}