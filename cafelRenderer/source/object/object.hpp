#pragma once

#if !defined(_CAFEL_OBJECT_HPP_)
#define _CAFEL_OBJECT_HPP_

#include <cafel/object/object.h>

CAFEL_NAMESPACE_BEGIN

class Object : public ObjectApi
{
public:	
	Object(const dVector &position_, const dVector &rotation_ = Vector_zero(double,3), const dVector &scale_ = Vector_one(double, 3))
		:ObjectApi(position_,rotation_,scale_){}
	Object(SceneNode *node):ObjectApi(node) {}

	Intersection intersect(const Ray &r);
	Interaction interact(Intersection &i, unsigned int depth) const;
	Color emitterEval(const Intersection &i) const;
	Color bsdfEval(const Intersection &i, unsigned int depth) const;
	dVector getRealPosition() const;
	dVector getRealScale() const;
	dVector getMin() const;
	dVector getMax() const;
	std::string tostring() const;
};

CAFEL_NAMESPACE_END

#endif 