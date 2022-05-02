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

	template<typename T>
	LGENG_MATH_FUNC Vector3<T> Quaternion::operator*(const Vector3<T>& v)
	{
		Vector3<T> quatVec(x, y, z);
		Vector3<T> uv(Math::Cross(quatVec, v));
		Vector3<T> uuv(Math::Cross(quatVec, uv));

		return v + ((uv * w) + uuv) * static_cast<T>(2);
	}

	LGENG_MATH_FUNC Quaternion Quaternion::operator/(float v)
	{
		return Quaternion(w / v, x / v, y / v, z / v);
	}

	LGENG_MATH_FUNC Quaternion Quaternion::operator*(const Quaternion& q)
	{
		return Quaternion(*this) *= q;
	}

	LGENG_MATH_FUNC Quaternion& Quaternion::operator*=(const Quaternion& q)
	{
		Quaternion p(*this);

		w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;
		x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
		y = p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z;
		z = p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x;
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