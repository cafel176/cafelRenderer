#pragma once

#if !defined(_CAFEL_COLOR_HPP_)
#define _CAFEL_COLOR_HPP_

#include <cafel/basic/basic.h>
#include <string>

CAFEL_NAMESPACE_BEGIN

#define Color_zero Color(0, 0, 0)
#define Color_one Color(1, 1, 1)

class Color : public Basic
{
public:
	Color(const Color& other) : r(other.r), g(other.g), b(other.b), a(other.a){}
	Color(double r_ = 0, double g_ = 0, double b_ = 0, double a_ = 0) : r(r_), g(g_), b(b_), a(a_){}

	Color& operator=(const Color& other)
	{
		r = other.r; g = other.g; b = other.b; a = other.a;
		return *this;
	}

	Color operator+(double other) const
	{
		return Color(r + other, g + other, b + other, a + other);
	}

	Color operator+(const Color &other) const
	{
		return Color(r + other.r, g + other.g, b + other.b, a + other.a);
	}

	Color operator-(double other) const
	{
		return Color(r - other, g - other, b - other, a - other);
	}

	Color operator-(const Color &other) const
	{
		return Color(r - other.r, g - other.g, b - other.b, a - other.a);
	}

	Color operator*(double other) const
	{
		return Color(r*other, g*other, b*other, a*other);
	}

	Color operator*(const Color &other) const
	{
		return Color(r * other.r, g * other.g, b * other.b, a * other.a);
	}

	Color operator/(double other) const
	{
		if (abs(other) == 0)
			return Color_zero;
		return Color(r / other, g / other, b / other, a / other);
	}

	Color operator/(const Color &other) const
	{
		return Color(other.r == 0 ? 0 : r / other.r, other.g == 0 ? 0 : g / other.g, other.b == 0 ? 0 : b / other.b, other.a == 0 ? 0 : a / other.a);
	}

	double getMax() 
	{
		return (r > g && r > b ? r : (g > b ? g : b));
	}

	operator std::string() const
	{ 
		return "(" + 
			std::to_string(r) + " , " + 
			std::to_string(g) + " , " + 
			std::to_string(b) + " , " + 
			std::to_string(a) + 
		")"; 
	}

	double r, g, b, a;
};

CAFEL_NAMESPACE_END

#endif 