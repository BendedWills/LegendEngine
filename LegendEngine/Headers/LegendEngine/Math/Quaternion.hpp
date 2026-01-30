#pragma once

#include <LegendEngine/Math/Vector3.hpp>

namespace LegendEngine
{
	class Quaternion
	{
	public:
		Quaternion();
		Quaternion(float w, float x, float y, float z);
		
		Quaternion(const Quaternion& ref);
		Quaternion operator=(const Quaternion& ref);

		template<typename T>
		Vector3<T> operator*(const Vector3<T>& value);

		Quaternion operator/(float value);
		Quaternion operator*(const Quaternion& value);
		Quaternion& operator*=(const Quaternion& value);
		
		/**
		 * @brief Converts the quaternion to Euler angles.
		 * 
		 * @returns The converted Euler angles.
		 */
		Vector3<float> GetEulerAngles();

		float w = 1.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
}

#include <LegendEngine/Math/Quaternion.inl>
