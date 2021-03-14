#pragma once

#if !defined(_CAFEL_DATA_MAT_HPP_)
#define _CAFEL_DATA_MAT_HPP_

#include <cafel/basic/vector.hpp>
#include <vector>

CAFEL_NAMESPACE_BEGIN

template<typename T>
class DataMat : public Basic
{
public:
	DataMat(){}

	DataMat(T t,unsigned int width, unsigned int height)
	{
		init(t, width, height);
	}

	DataMat(const DataMat& other, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		row = height;
		col = width;
		for (int i = 0; i < height; i++)
		{
			std::vector<T> v;
			std::vector<T> o = other[y + i];
			v.insert(v.end(), o.begin()+x, o.begin() + x+width);
			data.push_back(v);
		}
	}

	DataMat(const DataMat& other) :row(other.row), col(other.col), data(other.data) {}

	DataMat& operator=(const DataMat& other)
	{
		row = other.row; col = other.col; data = other.data;
		return *this;
	}

	void init(T t, unsigned int width, unsigned int height)
	{
		clear();
		row = height;
		col = width;
		for (int i = 0; i < height; i++)
		{
			std::vector<T> v(col,t);
			data.push_back(v);
		}
	}

	void clear()
	{
		data.clear();
		//data.swap(std::vector<std::vector<T>>(data));
	}

	T sum()
	{
		T re;
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++)
				re = re + data[i][j];
		return re;
	}

	void copyFrom(const DataMat other, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int width, unsigned int height)
	{
		for (int i = 0; i < width; i++) // col
			for (int j = 0; j < height; j++) // row
			{
				data[y1 + j][x1 + i] = other[y2 + j][x2 + i];
			}
	}

	std::vector<T>& operator[](int i)
	{
		if (i >= 0 && i < row)
			return data[i];
		else
			return data[0];
	}

	std::vector<T> operator[](int i) const
	{
		if (i >= 0 && i < row)
			return data[i];
		else
		{
			std::vector<T> v;
			return v;
		}
	}

	int row, col;

private:
	std::vector<std::vector<T>> data;
};

typedef DataMat<double> dDataMat;
typedef DataMat<float> fDataMat;
typedef DataMat<int> iDataMat;

typedef DataMat<iVector> ivDataMat;

CAFEL_NAMESPACE_END

#endif