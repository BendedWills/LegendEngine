#include <string.h>

namespace LegendEngine
{
	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T>::Matrix4x4()
	{}

	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T>::Matrix4x4(T value)
	{
		for (uint8_t i = 0; i < COL_COUNT; i++)
			c[i] = Vector4<T>(value);
	}

	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T>::Matrix4x4(Quaternion q)
	{
		// Construct a 4x4 matrix from a quaternion q

		T xx(q.x * q.x);
		T yy(q.y * q.y);
		T zz(q.z * q.z);
		T xz(q.x * q.z);
		T xy(q.x * q.y);
		T yz(q.y * q.z);
		T wx(q.w * q.x);
		T wy(q.w * q.y);
		T wz(q.w * q.z);

		c[0][0] = T(1) - T(2) * (yy + zz);
		c[0][1] = T(2) * (xy + wz);
		c[0][2] = T(2) * (xz - wy);

		c[1][0] = T(2) * (xy - wz);
		c[1][1] = T(1) - T(2) * (xx + zz);
		c[1][2] = T(2) * (yz + wx);

		c[2][0] = T(2) * (xz + wy);
		c[2][1] = T(2) * (yz - wx);
		c[2][2] = T(1) - T(2) * (xx + yy);

		c[3][3] = T(1);
	}

	// The ref is the one to copy from (I always forget that)
	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T>::Matrix4x4(const Matrix4x4& ref)
	{
		memcpy(c, ref.c, sizeof(c));
	}

	template<typename T>
	LGENG_MATH_FUNC Vector4<T>& Matrix4x4<T>::operator[](int index)
	{
		return c[index];
	}

	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T> Matrix4x4<T>::operator=(const Matrix4x4<T>& toSet)
	{
		memcpy(c, toSet.c, sizeof(c));
		return *this;
	}

	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4<T>& m)
	{
		return Multiply(m);
	}

	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T>& Matrix4x4<T>::operator*(T& m)
	{
		c[0] *= m;
		c[1] *= m;
		c[2] *= m;
		c[3] *= m;

		return *this;
	}

	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T>& Matrix4x4<T>::operator*=(const Matrix4x4<T>& m)
	{
		Matrix4x4<T> product = Multiply(m);
		memcpy(c, product.c, sizeof(c));

		return *this;
	}

	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T>& Matrix4x4<T>::operator*=(T m)
	{
		c[0] *= m;
		c[1] *= m;
		c[2] *= m;
		c[3] *= m;

		return *this;
	}

	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T> Matrix4x4<T>::MakeIdentity()
	{
		Matrix4x4<T> mat;
		mat.c[0] = Vector4<T>(1, 0, 0, 0);
		mat.c[1] = Vector4<T>(0, 1, 0, 0);
		mat.c[2] = Vector4<T>(0, 0, 1, 0);
		mat.c[3] = Vector4<T>(0, 0, 0, 1);

		return mat;
	}

	template<typename T>
	LGENG_MATH_FUNC Matrix4x4<T> Matrix4x4<T>::Multiply(Matrix4x4<T> m)
	{
		Matrix4x4<T> product;
		product.c[0] = Vector4f(
			c[0].x * m.c[0].x + c[1].x * m.c[0].y + c[2].x * m.c[0].z + c[3].x * m.c[0].w,
			c[0].y * m.c[0].x + c[1].y * m.c[0].y + c[2].y * m.c[0].z + c[3].y * m.c[0].w,
			c[0].z * m.c[0].x + c[1].z * m.c[0].y + c[2].z * m.c[0].z + c[3].z * m.c[0].w,
			c[0].w * m.c[0].x + c[1].w * m.c[0].y + c[2].w * m.c[0].z + c[3].w * m.c[0].w
		);
		product.c[1] = Vector4f(
			c[0].x * m.c[1].x + c[1].x * m.c[1].y + c[2].x * m.c[1].z + c[3].x * m.c[1].w,
			c[0].y * m.c[1].x + c[1].y * m.c[1].y + c[2].y * m.c[1].z + c[3].y * m.c[1].w,
			c[0].z * m.c[1].x + c[1].z * m.c[1].y + c[2].z * m.c[1].z + c[3].z * m.c[1].w,
			c[0].w * m.c[1].x + c[1].w * m.c[1].y + c[2].w * m.c[1].z + c[3].w * m.c[1].w
		);
		product.c[2] = Vector4f(
			c[0].x * m.c[2].x + c[1].x * m.c[2].y + c[2].x * m.c[2].z + c[3].x * m.c[2].w,
			c[0].y * m.c[2].x + c[1].y * m.c[2].y + c[2].y * m.c[2].z + c[3].y * m.c[2].w,
			c[0].z * m.c[2].x + c[1].z * m.c[2].y + c[2].z * m.c[2].z + c[3].z * m.c[2].w,
			c[0].w * m.c[2].x + c[1].w * m.c[2].y + c[2].w * m.c[2].z + c[3].w * m.c[2].w
		);
		product.c[3] = Vector4f(
			c[0].x * m.c[3].x + c[1].x * m.c[3].y + c[2].x * m.c[3].z + c[3].x * m.c[3].w,
			c[0].y * m.c[3].x + c[1].y * m.c[3].y + c[2].y * m.c[3].z + c[3].y * m.c[3].w,
			c[0].z * m.c[3].x + c[1].z * m.c[3].y + c[2].z * m.c[3].z + c[3].z * m.c[3].w,
			c[0].w * m.c[3].x + c[1].w * m.c[3].y + c[2].w * m.c[3].z + c[3].w * m.c[3].w
		);

		return product;
	}
}