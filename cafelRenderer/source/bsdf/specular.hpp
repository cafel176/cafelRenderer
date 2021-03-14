#pragma once

#if !defined(_CAFEL_SPECULAR_HPP_)
#define _CAFEL_SPECULAR_HPP_

#include <cafel/components/bsdf.h>

CAFEL_NAMESPACE_BEGIN

class Specular : public BSDF
{
public:
	Specular(SceneNode *node) :BSDF(node) {}
	Specular(std::shared_ptr<Sampler> sampler_) :BSDF(sampler_) {}

	Ray sample(const Intersection &i, unsigned int depeth);
	std::string tostring() const { return "Specular:(" + sampler->tostring() + ")"; }
	void generateCode(std::ostringstream &oss, int lightNum);
};

CAFEL_NAMESPACE_END

#endif 