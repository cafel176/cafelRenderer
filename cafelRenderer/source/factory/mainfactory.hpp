#pragma once

#if !defined(_CAFEL_MAINFACTORY_HPP_)
#define _CAFEL_MAINFACTORY_HPP_

#include <cafel/system/factory.h>

#include <object/sceneBuilder.hpp>
#include <object/camera.hpp>
#include <object/glSystem.hpp>

#include <factory/samplerfactory.hpp>
#include <factory/bsdffactory.hpp>
#include <factory/emitterfactory.hpp>
#include <factory/integratorfactory.hpp>
#include <factory/objectfactory.hpp>
#include <factory/shapefactory.hpp>

#include <factory/treefactories/kdfactory.hpp>

CAFEL_NAMESPACE_BEGIN

enum TreeType
{
	KD
};

class MainFactory : public Factory
{
public:
	static Element* get(SceneNode *node)
	{
		if (node->getName() == "bsdf")
			return BSDFFactory::get(node);
		else if (node->getName() == "emitter")
			return EmitterFactory::get(node);
		else if (node->getName() == "integrator")
			return IntegratorFactory::get(node);
		else if (node->getName() == "sampler")
			return SamplerFactory::get(node);
		else if (node->getName() == "shape")
			return ShapeFactory::get(node);
		else if (node->getName() == "object")
			return ObjectFactory::get(node);
		else
			return nullptr;
	}

	static TreeFactory* getTreeFactory(TreeType type)
	{
		switch (type)
		{
		case TreeType::KD: return new KdFactory(); break;
		default: return new KdFactory(); break;
		}
	}

	static SceneBuilderApi* getScene(SceneNode *node)
	{
		return new SceneBuilder(node);
	}

	static CameraApi* getCamera(SceneNode *node)
	{
		return new Camera(node);
	}

	static GLSystemApi* getGLSystem(SceneBuilderApi *scene,double scale = 1.0)
	{
		return new GLSystem(scene,scale);
	}

private:
	MainFactory();
};

CAFEL_NAMESPACE_END

#endif