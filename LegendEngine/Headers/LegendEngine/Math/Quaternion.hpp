#ifndef _LEGENDENGINE_MATH_QUATERNION_HPP
#define _LEGENDENGINE_MATH_QUATERNION_HPP

#include <LegendEngine/Math/Vector3.hpp>
#include <iostream>

namespace LegendEngine
{
	class Quaternion
	{
	public:
		Quaternion();
		Quaternion(float w, float x, float y, float z);
		// From Euler angles
		template<typename T>
		Quaternion(Vector3<T> vec);
		
		Quaternion(const Quaternion& ref);
		Quaternion operator=(const Quaternion& ref);

		Quaternion operator*(const Quaternion& value);
		Quaternion& operator*=(const Quaternion& value);
		
		template<typename T>
		void SetEulerAngles(Vector3<T> vec);
		/**
		 * @brief Converts the quaternion to Euler angles.
		 * 
		 * @returns The converted Euler angles.
		 */
		Vector3<float> GetEulerAngles();

		static Quaternion Normalize(Quaternion q);
		static float Dot(Quaternion q1, Quaternion q2);
		static float Length(Quaternion q);
		template<typename T>
		static Quaternion AngleAxis(float angle, Vector3<T> axis);

		float w = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
}

#include <LegendEngine/Math/Quaternion.inl>

#endif //_LEGENDENGINE_MATH_QUATERNION_HPP