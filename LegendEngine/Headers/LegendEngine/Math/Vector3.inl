namespace LegendEngine
{
	template<typename T>
	Vector3<T>::Vector3()
	{}

	template<typename T>
	Vector3<T>::Vector3(T value)
		:
		x(value),
		y(value),
		z(value)
	{}

	template<typename T>
	Vector3<T>::Vector3(T x, T y, T z)
		:
		x(x),
		y(y),
		z(z)
	{}

	template<typename T>
	Vector3<T>::Vector3(Vector2<T> vec)
	{
		x = vec.x;
		y = vec.y;
	}

	// The ref is the one to copy from (I always forget that)
	template<typename T>
	Vector3<T>::Vector3(const Vector3<T>& ref)
	{
		x = ref.x;
		y = ref.y;
		z = ref.z;
	}

	template<typename T>
	T& Vector3<T>::operator[](int index)
	{
		switch (index)
		{
			case 0: return x;
			case 1: return y;
			case 2: return z;
		}

		throw "Vector index out of range!";
		return x;
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator=(const Vector3<T>& toSet)
	{
		x = toSet.x;
		y = toSet.y;
		z = toSet.z;
		return *this;
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator+(const Vector3<T>& toAdd)
	{
		Vector3<T> toReturn;
		toReturn.x = x + toAdd.x;
		toReturn.y = y + toAdd.y;
		toReturn.z = z + toAdd.z;
		return toReturn;
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator-(const Vector3<T>& toAdd)
	{
		Vector3<T> toReturn;
		toReturn.x = x - toAdd.x;
		toReturn.y = y - toAdd.y;
		toReturn.z = z - toAdd.z;
		return toReturn;
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator*(const Vector3<T>& toAdd)
	{
		Vector3<T> product;
		product.x = x * toAdd.x;
		product.y = x * toAdd.y;
		product.z = z * toAdd.z;
		return product;
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator+(const T& value)
	{
		Vector3<T> product;
		product.x = value;
		product.y = value;
		product.z = value;
		return product;
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator-(const T& value)
	{
		Vector3<T> product;
		product.x = value;
		product.y = value;
		product.z = value;
		return product;
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator*(const T& value)
	{
		Vector3<T> product;
		product.x = x * value;
		product.y = y * value;
		product.z = z * value;
		return product;
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator/(const T& value)
	{
		Vector3<T> product;
		product.x = value;
		product.y = value;
		product.z = value;
		return product;
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator/(const Vector3<T>& toAdd)
	{
		Vector3<T> toReturn;
		toReturn.x = x / toAdd.x;
		toReturn.y = y / toAdd.y;
		toReturn.z = z / toAdd.z;
		return toReturn;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator+=(const Vector3<T>& toAdd)
	{
		x += toAdd.x;
		y += toAdd.y;
		z += toAdd.z;
		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator-=(const Vector3<T>& toAdd)
	{
		x -= toAdd.x;
		y -= toAdd.y;
		z -= toAdd.z;
		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator*=(const Vector3<T>& toAdd)
	{
		x *= toAdd.x;
		y *= toAdd.y;
		z *= toAdd.z;
		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator/=(const Vector3<T>& toAdd)
	{
		x /= toAdd.x;
		y /= toAdd.y;
		z /= toAdd.z;
		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator+=(const T& value)
	{
		x += value;
		y += value;
		z += value;
		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator-=(const T& value)
	{
		x -= value;
		y -= value;
		z -= value;
		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator*=(const T& value)
	{
		x *= value;
		y *= value;
		z *= value;
		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator/=(const T& value)
	{
		x /= value;
		y /= value;
		z /= value;
		return *this;
	}

	template<typename T>
	Vector3<T> Vector3<T>::Normalize(Vector3<T> v)
	{
		// Normalizing a vector is simply dividing it by its length so that
		// its length is one.
		float length = Length(v);
		Vector3<T> vec = Vector3<T>(
			v.x / length,
			v.y / length,
			v.z / length
		);
		return vec;
	}

	template<typename T>
	float Vector3<T>::Dot(Vector3<T> v1, Vector3<T> v2)
	{
		float product = v1.x * v2.x;
		product += v1.y * v2.y;
		product += v1.z * v2.z;
		return product;
	}

	template<typename T>
	Vector3<T> Vector3<T>::Cross(Vector3<T> v1, Vector3<T> v2)
	{
		return Vector3<T>(
			v1.y * v2.z - v1.z * v2.y,
			v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x
		);
	}

	template<typename T>
	float Vector3<T>::Length(Vector3<T> v)
	{
		return sqrt(Dot(v, v));
	}
}