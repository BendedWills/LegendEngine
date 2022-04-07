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
}