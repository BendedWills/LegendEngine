#include <LegendEngine/Math/Funcs.hpp>

#include <cmath>

namespace LegendEngine
{
	template<typename T>
	Quaternion::Quaternion(Vector3<T> vec)
	{
		SetEulerAngles(vec);
	}

	template<typename T>
	void Quaternion::SetEulerAngles(Vector3<T> vec)
	{
		Vector3<T> halfVec = vec / T(2);

		double cy = cos(vec.z);
		double sy = sin(vec.z);
		double cp = cos(vec.y);
		double sp = sin(vec.y);
		double cr = cos(vec.x);
		double sr = sin(vec.x);

		x = sr * cp * cy - cr * sp * sy;
		y = cr * sp * cy + sr * cp * sy;
		z = cr * cp * sy - sr * sp * cy;
		w = cr * cp * cy + sr * sp * sy;
	}
}