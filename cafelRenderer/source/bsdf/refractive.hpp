#pragma once

#if !defined(_CAFEL_REFRACTIVE_HPP_)
#define _CAFEL_REFRACTIVE_HPP_

#include <cafel/components/bsdf.h>

CAFEL_NAMESPACE_BEGIN

class Refractive : public BSDF
{
public:
	Refractive(SceneNode *node) :BSDF(node) {}
	Refractive(std::shared_ptr<Sampler> sampler_) :BSDF(sampler_) {}

	Ray sample(const Intersection &i, unsigned int depeth);
	std::string tostring() const { return "Refractive:(" + sampler->tostring() + ")"; }
	void generateCode(std::ostringstream &oss, int lightNum);
};

CAFEL_NAMESPACE_END

#endif 