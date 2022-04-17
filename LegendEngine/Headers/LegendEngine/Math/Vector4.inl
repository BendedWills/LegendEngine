namespace LegendEngine
{
	template<typename T>
	Vector4<T>::Vector4() 
	{}

	template<typename T>
	Vector4<T>::Vector4(T value)
		:
		x(value),
		y(value),
		z(value),
		w(value)
	{}

	template<typename T>
	Vector4<T>::Vector4(T x, T y, T z, T w)
		:
		x(x),
		y(y),
		z(z),
		w(w)
	{}

	template<typename T>
	Vector4<T>::Vector4(Vector2<T> vec)
	{
		x = vec.x;
		y = vec.y;
	}

	template<typename T>
	Vector4<T>::Vector4(Vector3<T> vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
	}

	// The ref is the one to copy from (I always forget that)
	template<typename T>
	Vector4<T>::Vector4(const Vector4<T>& ref)
	{
		x = ref.x;
		y = ref.y;
		z = ref.z;
		w = ref.w;
	}

	template<typename T>
	T& Vector4<T>::operator[](int index)
	{
		switch (index)
		{
			case 0: return x;
			case 1: return y;
			case 2: return z;
			case 3: return w;
		}

		throw "Vector index out of range!";
		return x;
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator=(const Vector4<T>& toSet)
	{
		x = toSet.x;
		y = toSet.y;
		z = toSet.z;
		w = toSet.w;
		return *this;
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator+(const Vector4<T>& toAdd)
	{
		Vector4<T> toReturn;
		toReturn.x = x + toAdd.x;
		toReturn.y = y + toAdd.y;
		toReturn.z = z + toAdd.z;
		toReturn.w = w + toAdd.w;
		return toReturn;
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator-(const Vector4<T>& toAdd)
	{
		Vector4<T> toReturn;
		toReturn.x = x - toAdd.x;
		toReturn.y = y - toAdd.y;
		toReturn.z = z - toAdd.z;
		toReturn.w = w - toAdd.w;
		return toReturn;
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator*(const Vector4<T>& toAdd)
	{
		Vector4<T> product;
		product.x = x * toAdd.x;
		product.y = x * toAdd.y;
		product.z = z * toAdd.z;
		product.w = w * toAdd.w;
		return product;
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator+(const T& value)
	{
		x += value;
		y += value;
		z += value;
		w += value;
		return *this;
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator-(const T& value)
	{
		x -= value;
		y -= value;
		z -= value;
		w -= value;
		return *this;
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator*(const T& value)
	{
		Vector4<T> product;
		product.x = x * value;
		product.y = y * value;
		product.z = z * value;
		product.w = w * value;
		return product;
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator/(const T& value)
	{
		x /= value;
		y /= value;
		z /= value;
		return *this;
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator/(const Vector4<T>& toAdd)
	{
		Vector4<T> toReturn;
		toReturn.x = x / toAdd.x;
		toReturn.y = y / toAdd.y;
		toReturn.z = z / toAdd.z;
		toReturn.w = w / toAdd.w;
		return toReturn;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator+=(const Vector4<T>& toAdd)
	{
		x += toAdd.x;
		y += toAdd.y;
		z += toAdd.z;
		w += toAdd.w;
		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator-=(const Vector4<T>& toAdd)
	{
		x -= toAdd.x;
		y -= toAdd.y;
		z -= toAdd.z;
		w -= toAdd.w;
		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator*=(const Vector4<T>& toAdd)
	{
		x *= toAdd.x;
		y *= toAdd.y;
		z *= toAdd.z;
		w *= toAdd.w;
		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator/=(const Vector4<T>& toAdd)
	{
		x /= toAdd.x;
		y /= toAdd.y;
		z /= toAdd.z;
		w /= toAdd.w;
		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator+=(const T& value)
	{
		x += value;
		y += value;
		z += value;
		w += value;
		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator-=(const T& value)
	{
		x -= value;
		y -= value;
		z -= value;
		w -= value;
		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator*=(const T& value)
	{
		x *= value;
		y *= value;
		z *= value;
		w *= value;
		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator/=(const T& value)
	{
		x /= value;
		y /= value;
		z /= value;
		w /= value;
		return *this;
	}

	template<typename T>
	Vector4<T> Vector4<T>::Normalize(Vector4<T> v)
	{
		// Normalizing a vector is simply dividing it by its length so that
		// its length is one.
		float length = Length(v);
		if (length == 0)
			return v;

		return Vector4<T>(
			v.x / length,
			v.y / length,
			v.z / length,
			v.w / length
		);
	}

	template<typename T>
	float Vector4<T>::Dot(Vector4<T> v1, Vector4<T> v2)
	{
		float product = v1.x * v2.x;
		product += v1.y * v2.y;
		product += v1.z * v2.z;
		product += v1.w * v2.w;
		return product;
	}

	template<typename T>
	float Vector4<T>::Length(Vector4<T> v)
	{
		return sqrt(Dot(v, v));
	}
}