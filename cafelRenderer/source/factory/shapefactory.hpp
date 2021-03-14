#pragma once

#if !defined(_CAFEL_SHAPEFACTORY_HPP_)
#define _CAFEL_SHAPEFACTORY_HPP_

#include <cafel/system/factory.h>
#include <shapes/sphere.hpp>
#include <shapes/Box.hpp>
#include <shapes/model.hpp>

CAFEL_NAMESPACE_BEGIN

enum ShapeType
{
	SPHERE,
	BOX,
	MODEL
};

class ShapeFactory : public Factory
{
public:
	static Shape* get(SceneNode *node)
	{
		ShapeType type = BOX;
		std::string str = node->getParam("type");
		if (str == "sphere")
			type = SPHERE;
		if (str == "box")
			type = BOX;
		if (str == "model")
			type = MODEL;
		return getShape(type, node);
	}

	static Shape* getShape(ShapeType type, SceneNode *node)
	{
		switch (type)
		{
		case ShapeType::BOX: return new Box(node); break;
		case ShapeType::SPHERE: return new Sphere(node); break;
		case ShapeType::MODEL: return new Model(node); break;
		default: return new Box(node); break;
		}
	}

private:
	ShapeFactory();
};

CAFEL_NAMESPACE_END

#endif 