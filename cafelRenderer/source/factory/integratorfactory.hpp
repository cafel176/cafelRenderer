#pragma once

#if !defined(_CAFEL_INTEGRATOR_FACTORY_HPP_)
#define _CAFEL_INTEGRATOR_FACTORY_HPP_

#include <cafel/system/factory.h>
#include <integrators/path.hpp>

CAFEL_NAMESPACE_BEGIN

enum IntegratorType
{
	PATH
};

class IntegratorFactory : public Factory
{
public:
	static Integrator* get(SceneNode *node)
	{
		IntegratorType type = PATH;
		std::string str = node->getParam("type");
		if (str == "path")
			type = PATH;
		return getIntegrator(type, node);
	}

	static Integrator* getIntegrator(IntegratorType type, SceneNode *node)
	{
		switch (type)
		{
		case IntegratorType::PATH: return new Path(node); break;
		default: return new Path(node); break;
		}
	}

private:
	IntegratorFactory();
};

CAFEL_NAMESPACE_END

#endif 