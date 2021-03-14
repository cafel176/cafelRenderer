#pragma once

#if !defined(_CAFEL_BSDFFACTORY_HPP_)
#define _CAFEL_BSDFFACTORY_HPP_

#include <cafel/system/sceneNode.h>
#include <cafel/system/factory.h>
#include <bsdf/diffuse.hpp>
#include <bsdf/specular.hpp>
#include <bsdf/refractive.hpp>
#include <bsdf/mdlbsdf.hpp>

CAFEL_NAMESPACE_BEGIN

enum BSDFType
{
	DIFFUSE,
	SPECULAR,
	REFRACTIVE,
	MDLBSDF
};

class BSDFFactory : public Factory
{
public:
	static BSDF* get(SceneNode *node)
	{
		BSDFType type = DIFFUSE;
		std::string str = node->getParam("type");
		if (str == "diffuse")
			type = DIFFUSE;
		else if (str == "specular")
			type = SPECULAR;
		else if (str == "refractive")
			type = REFRACTIVE;
		else if (str == "mdlbsdf")
			type = MDLBSDF;
		return getBSDF(type, node);
	}

	static BSDF* getBSDF(BSDFType type, SceneNode *node)
	{
		switch (type)
		{
		case BSDFType::DIFFUSE: return new Diffuse(node); break;
		case BSDFType::SPECULAR: return new Specular(node); break;
		case BSDFType::REFRACTIVE: return new Refractive(node); break;
		case BSDFType::MDLBSDF: return new MdlBSDF(node); break;
		default: return new Diffuse(node); break;
		}
	}
private:
	BSDFFactory();
};

CAFEL_NAMESPACE_END

#endif 