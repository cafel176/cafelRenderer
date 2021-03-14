#pragma once

#if !defined(_CAFEL_OBJECT_FACTORY_HPP_)
#define _CAFEL_OBJECT_FACTORY_HPP_

#include <cafel/system/factory.h>
#include <object/object.hpp>

CAFEL_NAMESPACE_BEGIN

class ObjectFactory : public Factory
{
public:
	static ObjectApi* get(SceneNode *node)
	{
		return new Object(node);
	}

	static ObjectApi* getObject(const dVector &p, const dVector &r = Vector_zero(double,3), const dVector &s = Vector_one(double, 3))
	{
		return new Object(p,r,s);
	}

private:
	ObjectFactory();
};

CAFEL_NAMESPACE_END

#endif 