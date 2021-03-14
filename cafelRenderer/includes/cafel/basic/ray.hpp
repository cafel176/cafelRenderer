#pragma once

#if !defined(_CAFEL_RAY_HPP_)
#define _CAFEL_RAY_HPP_

#include <cafel/basic/vector.hpp>

CAFEL_NAMESPACE_BEGIN

#define t_nohit -1

class Ray : public Basic
{
public:
	Ray(){}
	Ray(const dVector &o, const dVector &d, double priority = 1.0f):origin(o),direction(d), priority(priority){}
	Ray(const Ray &r) :origin(r.origin), direction(r.direction), priority(r.priority) {}

	Ray& operator=(const Ray& other)
	{
		origin = other.origin; direction = other.direction; priority = other.priority;
		return *this;
	}

	double priority;
	dVector direction, origin;
};

CAFEL_NAMESPACE_END

#endif 