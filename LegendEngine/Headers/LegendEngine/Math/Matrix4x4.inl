namespace LegendEngine
{
	template<typename T>
	Matrix4x4<T>::Matrix4x4() 
	{}

	template<typename T>
	Matrix4x4<T>::Matrix4x4(T value)
	{
		for (uint8_t i = 0; i < COL_COUNT; i++)
			c[i] = Vector4<T>(value);
	}

	// The ref is the one to copy from (I always forget that)
	template<typename T>
	Matrix4x4<T>::Matrix4x4(const Matrix4x4& ref)
	{
		memcpy(c, ref.c, sizeof(c));
	}

	template<typename T>
	Vector4<T>& Matrix4x4<T>::operator[](int index)
	{
		return c[index];
	}

	template<typename T>
	Matrix4x4<T> Matrix4x4<T>::operator=(const Matrix4x4<T>& toSet)
	{
		memcpy(c, toSet.c, sizeof(c));
		return *this;
	}

	template<typename T>
	Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4<T>& m)
	{
		return Multiply(m);
	}

	template<typename T>
	Matrix4x4<T>& Matrix4x4<T>::operator*(T& m)
	{
		c[0] *= m;
		c[1] *= m;
		c[2] *= m;
		c[3] *= m;

		return *this;
	}

	template<typename T>
	Matrix4x4<T>& Matrix4x4<T>::operator*=(const Matrix4x4<T>& m)
	{
		Matrix4x4<T> product = Multiply(m);
		memcpy(c, product.c, sizeof(c));

		return *this;
	}

	template<typename T>
	Matrix4x4<T>& Matrix4x4<T>::operator*=(T m)
	{
		c[0] *= m;
		c[1] *= m;
		c[2] *= m;
		c[3] *= m;

		return *this;
	}

	template<typename T>
	Matrix4x4<T> Matrix4x4<T>::Scale(Matrix4x4<T> m, Vector3<T> scale)
	{
		Matrix4x4<T> product;
		product.c[0] = m.c[0] * scale.x;
		product.c[1] = m.c[1] * scale.y;
		product.c[2] = m.c[2] * scale.z;
		product.c[3] = m.c[3];

		return product;
	}

	template<typename T>
	Matrix4x4<T> Matrix4x4<T>::Rotate(Matrix4x4<T> m, T a, Vector3<T> rotation)
	{
		T ca = cos(a);
		T sa = sin(a);

		Vector3<T> axis = Vector3<T>::Normalize(rotation);
		Vector3<T> temp(axis * (T(1) - ca));

		Matrix4x4<T> product;
		product[0][0] = ca + temp[0] * axis[0];
		product[0][1] = temp[0] * axis[1] + sa * axis[2];
		product[0][2] = temp[0] * axis[2] - sa * axis[1];

		product[1][0] = temp[1] * axis[0] - sa * axis[2];
		product[1][1] = ca + temp[1] * axis[1];
		product[1][2] = temp[1] * axis[2] + sa * axis[0];

		product[2][0] = temp[2] * axis[0] + sa * axis[1];
		product[2][1] = temp[2] * axis[1] - sa * axis[0];
		product[2][2] = ca + temp[2] * axis[2];

		Matrix4x4<T> result;
		result.c[0] = m.c[0] * product.c[0].x + m.c[1] * product.c[0].y + m.c[2] * product.c[0].z;
		result.c[1] = m.c[0] * product.c[1].x + m.c[1] * product.c[1].y + m.c[2] * product.c[1].z;
		result.c[2] = m.c[0] * product.c[2].x + m.c[1] * product.c[2].y + m.c[2] * product.c[2].z;
		result.c[3] = m.c[3];

		return result;
	}

	template<typename T>
	Matrix4x4<T> Matrix4x4<T>::Translate(Matrix4x4<T> m, Vector3<T> pos)
	{
		Matrix4x4<T> product = m;
		product.c[3] = m[0] * pos.x + m[1] * pos.y + m[2] * pos.z + m[3];

		return product;
	}

	template<typename T>
	Matrix4x4<T> Matrix4x4<T>::MakeIdentity()
	{
		Matrix4x4<T> mat;
		mat.c[0] = Vector4<T>(1, 0, 0, 0);
		mat.c[1] = Vector4<T>(0, 1, 0, 0);
		mat.c[2] = Vector4<T>(0, 0, 1, 0);
		mat.c[3] = Vector4<T>(0, 0, 0, 1);

		return mat;
	}

	template<typename T>
	Matrix4x4<T> Matrix4x4<T>::Multiply(Matrix4x4<T> m)
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