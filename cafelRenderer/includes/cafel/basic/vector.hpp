#pragma once

#if !defined(_CAFEL_VECTOR_HPP_)
#define _CAFEL_VECTOR_HPP_

#include <cafel/basic/basic.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <string>

CAFEL_NAMESPACE_BEGIN

#define Vector_single(T,V,col) Vector<T,col>(V,V,V,V)
#define Vector_zero(T,col) Vector<T,col>(0,0,0,0)
#define Vector_one(T,col) Vector<T,col>(1,1,1,1)

template<typename T,int col>
class Vector : public Basic
{
public:
	Vector(T x_ = 0, T y_ = 0, T z_ = 0, T w_ = 0):x(x_),y(y_),z(z_),w(w_){}
	Vector(T* arr)
	{
		if (arr != nullptr)
		{
			x = arr[0];
			y = arr[1];
			z = arr[2];
			w = arr[3];
		}
	}

	Vector(const Vector& other) :x(other.x), y(other.y), z(other.z), w(other.w),main(other.main){}

	Vector& operator=(const Vector& other)
	{
		x = other.x; y = other.y; z = other.z; w = other.w; main = other.main; 
		return *this;
	}

	Vector operator+(T b) const
	{
		return Vector(x + b, y + b, z + b, w + b);
	}

	Vector operator+(const Vector &b) const
	{
		return Vector(x + b.x, y + b.y, z + b.z, w + b.w);
	}

	Vector operator-(T b) const
	{
		return Vector(x - b, y - b, z - b, w - b);
	}

	Vector operator-(const Vector &b) const
	{
		return Vector(x - b.x, y - b.y, z - b.z, w - b.w);
	}

	Vector operator*(T b) const
	{
		return Vector(x*b, y*b, z * b, w * b);
	}

	Vector operator*(const Vector &b) const
	{
		return Vector(x * b.x, y * b.y, z * b.z, w * b.w);
	}

	Vector operator/(T b) const
	{
		if (abs(b) == 0)
			return Vector_zero(T,col);
		return Vector(x / b, y / b, z / b, w / b);
	}

	Vector operator/(const Vector &b) const
	{
		return Vector(b.x==0?0:x / b.x, b.y == 0 ? 0 : y / b.y, (b.z == 0 ? 0 : z / b.z), (b.w == 0 ? 0 : w / b.w));
	}

	Vector operator%(const Vector &b) const
	{
		if (col < 3)
			return Vector(x * b.y - y * b.x,0,0,0);
		else if (col == 3)
			return Vector(y * b.z - z * b.y,z * b.x - x * b.z,x * b.y - y * b.x,0);

		T w_ = ((abs(w) < eps) ? 0 : w);
		T b_w = ((abs(b.w) < eps) ? 0 : b.w);
		return Vector(
			w_ * b.x + x * b_w + y * b.z - z * b.y,
			w_ * b.y + y * b_w + z * b.x - x * b.z,
			w_ * b.z + z * b_w + x * b.y - y * b.x,
			(abs(w_)<eps||abs(b_w)<eps)?0:(w_ * b_w - x * b.x - y * b.y - z * b.z)
		);
	}//cross

	bool operator>(const Vector &b)
	{
		if (main >= 0 && main < col && b.main >= 0 && b.main < col)
		{
			return (*this)[main] > b[b.main];
		}
		else
		{
			for (int i = 0; i < col; i++)
			{
				if ((*this)[i] > b[i])
					return true;
				else if ((*this)[i] < b[i])
					return false;
			}
		}
		return false;
	}

	bool operator==(const Vector &b)
	{
		if (x==b.x && y==b.y && (col>2?z==b.z:true) && (col > 3 ? w == b.w : true))
		{
			return true;
		}
		else
			return false;
	}

	T dot(const Vector &b) const
	{
		if (col < 3)
			return x * b.x + y * b.y;
		else if (col == 3)
			return x * b.x + y * b.y + z * b.z;

		return x * b.x + y * b.y + z * b.z + w * b.w;
	}

	int indexOf(T t,bool abs_ = false) const
	{
		if (abs(t - (abs_?abs(x):x)) < eps)
			return 0;
		else if (abs(t - (abs_ ? abs(y) : y)) < eps)
			return 1;
		else if (abs(t - (abs_ ? abs(z) : z)) < eps && col>2)
			return 2;
		else if (abs(t - (abs_ ? abs(w) : w)) < eps && col > 3)
			return 3;
		else
			return -1;
	}

	T length() const
	{
		if (col < 3)
			return sqrt(x * x + y * y);
		else if (col == 3)
			return sqrt(x * x + y * y + z * z);

		return sqrt(x * x + y * y + z * z + w * w);
	}

	Vector& norm()
	{
		*this = *this * (1 / length());
		return *this;
	}

	operator std::string() const
	{
		return "(" +
			std::to_string(x) + " , " +
			std::to_string(y) + " , " +
			std::to_string(z) + " , " +
			std::to_string(w) +
			")";
	}

	T& operator[](int index)
	{
		switch (index)
		{
		case 0:return x; break;
		case 1:return y; break;
		case 2:if (col > 2) return z; break;
		case 3:if (col > 3) return w; break;
		}
		return w;
	}

	T operator[](int index) const
	{
		switch (index)
		{
		case 0:return x; break;
		case 1:return y; break;
		case 2:if (col > 2) return z; break;
		case 3:if (col > 3) return w; break;
		}
		return NULL;
	}

	T x, y, z, w;
	int main = -1;
};

template<typename T, int col>
bool operator<(const Vector<T, col> v1, const Vector<T, col> v2)
{
	if (v1.main >= 0 && v1.main < col && v2.main >= 0 && v2.main < col)
	{
		return v1[v1.main] < v2[v2.main];
	}
	else
	{
		for (int i = 0; i < col; i++)
		{
			if (v1[i] < v2[i])
				return true;
			else if (v1[i] > v2[i])
				return false;
		}
	}
	return false;
}

typedef Vector<double, 2> dVector2;
typedef Vector<float, 2> fVector2;
typedef Vector<int, 2> iVector2;
typedef Vector<double,3> dVector;
typedef Vector<float, 3> fVector;
typedef Vector<int,3> iVector;
typedef Vector<double, 4> dVector4;
typedef Vector<float, 4> fVector4;
typedef Vector<int, 4> iVector4;

CAFEL_NAMESPACE_END

#endif 