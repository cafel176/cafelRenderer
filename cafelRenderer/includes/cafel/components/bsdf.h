#pragma once
#if !defined(_CAFEL_BSDF_H_)
#define _CAFEL_BSDF_H_

#include <cafel/components/sampler.h>
#include <factory/samplerfactory.hpp>
#include <sampler/rawcolor.hpp>
#include <memory>
#include <sstream>

CAFEL_NAMESPACE_BEGIN

class BSDF : public CAFEL_ELEMENT
{
public:
	BSDF(SceneNode *node):Element(node)
	{
		auto n = node->getChildFirst("sampler");
		sampler = std::shared_ptr<Sampler>(static_cast<Sampler*>(SamplerFactory::get(n)));
	}

	BSDF(std::shared_ptr<Sampler> sampler_) {sampler = sampler_; }
	virtual ~BSDF() {}

	virtual Color eval(const Intersection &i, unsigned int depth)
	{
		return sampler->eval(i);
	}

	virtual Ray sample(const Intersection &i, unsigned int depth) = 0;

	std::shared_ptr<Sampler> getSampler() { return sampler; }

	virtual std::string tostring() const = 0;
	virtual void generateCode(std::ostringstream &oss,int lightNum)
	{
		oss << "#version 330 core" << std::endl
			<< "out vec4 FragColor;" << std::endl
			<< std::endl
			<< "in VS_OUT {" << std::endl
			<< "    vec3 FragPos;" << std::endl
			<< "    vec3 Normal;" << std::endl
			<< "    vec2 TexCoords;" << std::endl
			<< "} fs_in;" << std::endl
			<< std::endl
			<< "uniform sampler2D sampler;" << std::endl
			<< "#define NR_POINT_LIGHTS " << lightNum << std::endl
			<< "uniform vec3 lightPos[NR_POINT_LIGHTS];" << std::endl
			<< "uniform vec3 viewPos;" << std::endl
			<< std::endl;
	}

protected:
	std::shared_ptr<Sampler> sampler;
};

CAFEL_NAMESPACE_END

#endif 
