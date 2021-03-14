#pragma once

#if !defined(_CAFEL_SAMPLERFACTORY_HPP_)
#define _CAFEL_SAMPLERFACTORY_HPP_

#include <cafel/system/factory.h>
#include <sampler/rawcolor.hpp>
#include <sampler/texture.hpp>
#include <sampler/textureSynthesis.hpp>

CAFEL_NAMESPACE_BEGIN

enum SamplerType
{
	COLOR,
	TEXTURE,
	TEXTURE_SYNTHESIS
};

class SamplerFactory : public Factory
{
public:
	static Sampler* get(SceneNode *node)
	{
		SamplerType type = COLOR;
		std::string str = node->getParam("type");
		if (str == "rawcolor")
			type = COLOR;
		if (str == "texture")
			type = TEXTURE;
		if (str == "textureSynthesis")
			type = TEXTURE_SYNTHESIS;
		return getSampler(type, node);
	}

	static Sampler* getSampler(SamplerType type, SceneNode *node)
	{
		switch (type)
		{
			case SamplerType::COLOR: return new RawColor(node); break;
			case SamplerType::TEXTURE: return new TextureSampler(node); break;
			case SamplerType::TEXTURE_SYNTHESIS: return new TextureSynthesis(node); break;
			default: return new RawColor(node); break;
		}
	}

private:
	SamplerFactory();
};

CAFEL_NAMESPACE_END

#endif 