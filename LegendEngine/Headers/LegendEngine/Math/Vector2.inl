namespace LegendEngine
{
	template<typename T>
	Vector2<T>::Vector2() 
	{}

	template<typename T>
	Vector2<T>::Vector2(T value)
		:
		x(value),
		y(value)
	{}

	template<typename T>
	Vector2<T>::Vector2(T x, T y)
		:
		x(x),
		y(y)
	{}

	// The ref is the one to copy from (I always forget that)
	template<typename T>
	Vector2<T>::Vector2(const Vector2<T>& ref)
	{
		x = ref.x;
		y = ref.y;
	}

	template<typename T>
	T& Vector2<T>::operator[](int index)
	{
		switch (index)
		{
			case 0: return x;
			case 1: return y;
		}

		throw "Vector index out of range!";
		return x;
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator=(const Vector2<T>& toSet)
	{
		x = toSet.x;
		y = toSet.y;
		return *this;
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator+(const Vector2<T>& toAdd)
	{
		Vector2<T> toReturn;
		toReturn.x = x + toAdd.x;
		toReturn.y = y + toAdd.y;
		return toReturn;
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator-(const Vector2<T>& toAdd)
	{
		Vector2<T> toReturn;
		toReturn.x = x - toAdd.x;
		toReturn.y = y - toAdd.y;
		return toReturn;
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator*(const Vector2<T>& toAdd)
	{
		Vector2<T> product;
		product.x = x * toAdd.x;
		product.y = y * toAdd.y;
		return product;
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator/(const Vector2<T>& toAdd)
	{
		Vector2<T> toReturn;
		toReturn.x = x / toAdd.x;
		toReturn.y = y / toAdd.y;
		return toReturn;
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator+(const T& value)
	{
		x += value;
		y += value;
		return *this;
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator-(const T& value)
	{
		x -= value;
		y -= value;
		return *this;
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator*(const T& value)
	{
		Vector2<T> product;
		product.x *= x * value;
		product.y *= y * value;
		return product;
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator/(const T& value)
	{
		x /= value;
		y /= value;
		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator+=(const Vector2<T>& toAdd)
	{
		x += toAdd.x;
		y += toAdd.y;
		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& toAdd)
	{
		x -= toAdd.x;
		y -= toAdd.y;
		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator*=(const Vector2<T>& toAdd)
	{
		x *= toAdd.x;
		y *= toAdd.y;
		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator/=(const Vector2<T>& toAdd)
	{
		x /= toAdd.x;
		y /= toAdd.y;
		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator+=(const T& value)
	{
		x += value;
		y += value;
		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator-=(const T& value)
	{
		x -= value;
		y -= value;
		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator*=(const T& value)
	{
		x *= value;
		y *= value;
		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator/=(const T& value)
	{
		x /= value;
		y /= value;
		return *this;
	}

	template<typename T>
	Vector2<T> Vector2<T>::Normalize(Vector2<T> v)
	{
		// Normalizing a vector is simply dividing it by its length so that
		// its length is one.
		float length = Length(v);
		return Vector2<T>(
			v.x / length,
			v.y / length
		);
	}

	template<typename T>
	float Vector2<T>::Dot(Vector2<T> v1, Vector2<T> v2)
	{
		float product = v1.x * v2.x;
		product += v1.y * v2.y;
		return product;
	}

	template<typename T>
	float Vector2<T>::Length(Vector2<T> v)
	{
		return sqrt(Dot(v, v));
	}
}