#pragma once

#if !defined(_CAFEL_MATRIX_HPP_)
#define _CAFEL_MATRIX_HPP_

#include <cafel/basic/vector.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <memory>

CAFEL_NAMESPACE_BEGIN

template<typename T, int row, int col>
class Matrix : public Basic
{
public:
	Matrix(T t)
	{
		data = std::shared_ptr<T*[row]>(new T*[row]);
		for (int i = 0; i < row; i++)
		{
			data[i] = new T[col];
			for (int j = 0; j < col; j++)
					data[i][j] = t;
		}
	}

	Matrix()
	{		
		data = std::shared_ptr<T*[row]>(new T*[row]);
		for (int i = 0; i < row; i++)
		{
			data[i] = new T[col];
			for (int j = 0; j < col; j++)
				if(i==j)
					data[i][j] = 1;
				else
					data[i][j] = 0;
		}
	}

	Matrix(const Matrix& other) :data(other.data){}

	Matrix& operator=(const Matrix& other)
	{
		data = other.data;
		return *this;
	}

	Vector<T,row> operator*(const Vector<T, col> &v) const
	{
		Vector<T, col> re;
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++)
				re[i] += data[i][j] * v[j];
			
		return re;
	}

	template<int col2>
	Matrix<T,row,col2> operator*(const Matrix<T,col, col2> &m2) const
	{
		Matrix<T, row, col2> Result(0);
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col2; j++)
				for (int k = 0; k < col; k++)
					Result[i][j] += data[i][k] * m2[k][j];

		return Result;
	}
	
	T*& operator[](int i)
	{ 
		if (i >= 0 && i < row)
			return data[i];
		else
			return data[0];
	}

	T* operator[](int i) const
	{
		if (i >= 0 && i < row)
			return data[i];
		else
			return nullptr;
	}

	operator std::string() const
	{
		std::string str = "(";
		for (int i = 0; i < row; i++)
		{
			if (i > 0)
				str += ",";
			str += std::to_string(i) + ":(";
			for (int j = 0; j < col; j++)
			{
				if (j > 0)
					str += ",";
				str += data[i][j];
			}
			str += ")";
		}
		str += ")";
		return str;
	}

	Matrix<T,4,4> static lookAt(const Vector<T,3>& eye, const Vector<T, 3>& center, const Vector<T, 3>& up)
	{
		Vector<T, 3> f(center - eye);
		f.norm();
		Vector<T, 3> s(f % up);
		s.norm();
		Vector<T, 3> u(s % f);

		Matrix<T, 4, 4> Result;
		Result[0][0] = s.x;
		Result[1][0] = s.y;
		Result[2][0] = s.z;
		Result[0][1] = u.x;
		Result[1][1] = u.y;
		Result[2][1] = u.z;
		Result[0][2] = -f.x;
		Result[1][2] = -f.y;
		Result[2][2] = -f.z;
		Result[3][0] = s.dot(eye)*-1;
		Result[3][1] = u.dot(eye)*-1;
		Result[3][2] = f.dot(eye);
		return Result;
	}

	Matrix<T, 4, 4> static perspective(T fovy, T aspect, T zNear, T zFar)
	{
		const T tanHalfFovy = tan(fovy / 2.0);

		Matrix<T, 4, 4> Result(0);
		Result[0][0] = 1.0 / (aspect * tanHalfFovy);
		Result[1][1] = 1.0 / (tanHalfFovy);
		Result[2][2] = -(zFar + zNear) / (zFar - zNear);
		Result[2][3] = -1.0;
		Result[3][2] = -(2.0 * zFar * zNear) / (zFar - zNear);
		return Result;
	}

	Matrix<T, 4, 4> static translate(const Matrix<T, 4, 4>& m, const Vector<T, 3>& v)
	{
		Matrix<T, 4, 4> Result(m);
		Vector<T, 3> dv0(m[0]);
		Vector<T, 3> dv1(m[1]);
		Vector<T, 3> dv2(m[2]);
		Vector<T, 3> dv3(m[3]);
		Vector<T, 3> re = dv0 * v[0] + dv1 * v[1] + dv2 * v[2] + dv3;
		for(int i=0;i<4;i++)
			Result[3][i] = re[i];
		return Result;
	}

	Matrix<T, 4, 4> static scale(const Matrix<T, 4, 4>& m, const Vector<T, 3>& v)
	{
		Matrix<T, 4, 4> Result(m);
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				Result[i][j] *= v[j];

		return Result;
	}

	Matrix<T, 4, 4> static rotate(const Matrix<T, 4, 4>& m,T angel, const Vector<T, 3>& v)
	{
		T const a = angle;
		T const c = cos(a);
		T const s = sin(a);

		Vector<T, 3> qw = v;
		qw.norm();
		Vector<T, 3> axis(qw);
		Vector<T, 3> temp(axis * (T(1) - c));

		Matrix<T, 4, 4> Rotate(0);
		Rotate[0][0] = c + temp[0] * axis[0];
		Rotate[0][1] = 0 + temp[0] * axis[1] + s * axis[2];
		Rotate[0][2] = 0 + temp[0] * axis[2] - s * axis[1];

		Rotate[1][0] = 0 + temp[1] * axis[0] - s * axis[2];
		Rotate[1][1] = c + temp[1] * axis[1];
		Rotate[1][2] = 0 + temp[1] * axis[2] + s * axis[0];

		Rotate[2][0] = 0 + temp[2] * axis[0] + s * axis[1];
		Rotate[2][1] = 0 + temp[2] * axis[1] - s * axis[0];
		Rotate[2][2] = c + temp[2] * axis[2];

		Matrix<T, 4, 4> Result(0);
		Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
		Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
		Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
		Result[3] = m[3];
		return Result;
	}

private:
	std::shared_ptr<T*[row]> data;
};

typedef Matrix<double, 2, 2> dMatrix2;
typedef Matrix<float, 2, 2> fMatrix2;
typedef Matrix<int, 2, 2> iMatrix2;
typedef Matrix<double,3,3> dMatrix;
typedef Matrix<float, 3, 3> fMatrix;
typedef Matrix<int,3,3> iMatrix;
typedef Matrix<double, 4, 4> dMatrix4;
typedef Matrix<float, 4, 4> fMatrix4;
typedef Matrix<int, 4, 4> iMatrix4;

CAFEL_NAMESPACE_END

#endif 