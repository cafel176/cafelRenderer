#pragma once

#if !defined(_CAFEL_INTERACTION_HPP_)
#define _CAFEL_INTERACTION_HPP_

#include <cafel/basic/ray.hpp>
#include <cafel/basic/color.hpp>

CAFEL_NAMESPACE_BEGIN

class Interaction : public Basic
{
public:
	Interaction(Ray ray, const Color &color = Color_zero, const Color &emission = Color_zero):ray(ray), color(color),emission(emission) {}

	Interaction(const Interaction &itr):ray(itr.ray), color(itr.color), emission(itr.emission) {}

	Interaction& operator=(const Interaction& itr)
	{
		ray = itr.ray; color = itr.color; emission = itr.emission;
		return *this;
	}

	Color emission;
	Color color;
	Ray ray;
};

CAFEL_NAMESPACE_END

#endif 