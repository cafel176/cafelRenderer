#pragma once

#if !defined(_CAFEL_EMITTERFACTORY_HPP_)
#define _CAFEL_EMITTERFACTORY_HPP_

#include <cafel/system/factory.h>
#include <emitter/light.hpp>

CAFEL_NAMESPACE_BEGIN

enum EmitterType
{
	LIGHT
};

class EmitterFactory : public Factory
{
public:
	static Emitter* get(SceneNode *node)
	{
		EmitterType type = LIGHT;
		std::string str = node->getParam("type");
		if (str == "light")
			type = LIGHT;
		return getEmitter(type, node);
	}

	static Emitter* getEmitter(EmitterType type, SceneNode *node)
	{
		switch (type)
		{
		case EmitterType::LIGHT: return new Light(node); break;
		default: return new Light(node); break;
		}
	}

private:
	EmitterFactory();
};

CAFEL_NAMESPACE_END

#endif 