#pragma once

#if !defined(_CAFEL_DIFFUSE_HPP_)
#define _CAFEL_DIFFUSE_HPP_

#include <cafel/components/bsdf.h>

CAFEL_NAMESPACE_BEGIN

class Diffuse : public BSDF
{
public:
	Diffuse(SceneNode *node) :BSDF(node) {}
	Diffuse(std::shared_ptr<Sampler> sampler_):BSDF(sampler_) {}

	Ray sample(const Intersection &i, unsigned int depeth);
	std::string tostring() const { return "Diffuse:("+ sampler ->tostring()+")"; }
	void generateCode(std::ostringstream &oss, int lightNum);
};

CAFEL_NAMESPACE_END

#endif 