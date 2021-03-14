#pragma once

#if !defined(_CAFEL_SAMPLER_H_)
#define _CAFEL_SAMPLER_H_

#include <cafel/system/element.h>
#include <cafel/basic/color.hpp>
#include <cafel/basic/intersection.hpp>
#include <sstream>

CAFEL_NAMESPACE_BEGIN

class Sampler : public CAFEL_ELEMENT
{
public:
	Sampler(){}
	Sampler(SceneNode *node) :Element(node) {}
	virtual ~Sampler() {}

	virtual Color eval(double u, double v) = 0;
	virtual Color eval(const dVector2 &i) = 0;
	virtual Color eval(const Intersection &i) = 0;
	virtual std::string tostring() const = 0;
	virtual void generateCode(std::ostringstream &oss) = 0;
};

CAFEL_NAMESPACE_END

#endif 
