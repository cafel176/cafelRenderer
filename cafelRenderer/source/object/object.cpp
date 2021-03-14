#include <object/object.hpp>

CAFEL_NAMESPACE_BEGIN

std::string Object::tostring() const
{
	std::string str = "Object(\n\tposition:" + std::string(position);
	/*
	if (shape != nullptr)
		str += "\n\tshape:" + shape->tostring();
	if (bsdf != nullptr)
		str += "\n\tbsdf:" + bsdf->tostring();
	*/
	str += ")";
	return str;
}

Intersection Object::intersect(const Ray &r)
{
	if (shape == nullptr)
		return Intersection(t_nohit,r);
	else
	{
		return shape->intersect(r, position, rotation, scale);
	}
}

Color Object::emitterEval(const Intersection &i) const
{
	if (emitter == nullptr)
		return Color_zero;
	else
		return emitter->eval(i);
}

Color Object::bsdfEval(const Intersection &i, unsigned int depth) const
{
	if (bsdf == nullptr)
		return Color_zero;
	else
		return bsdf->eval(i,depth);
}

Interaction Object::interact(Intersection &its, unsigned int depth) const
{
	Ray ray = its.ray;
	if (bsdf != nullptr)
	{
		ray = bsdf->sample(its, depth);			
	}
	its.out_ray = ray;

	return Interaction(ray, bsdfEval(its,depth), emitterEval(its));
}

dVector Object::getRealPosition() const
{
	return shape->getRealPosition(position, rotation);
}

dVector Object::getRealScale() const
{
	return shape->getRealScale(scale);
}

dVector Object::getMin() const
{
	return shape->getMin(position, rotation,scale);
}

dVector Object::getMax() const
{
	return shape->getMax(position, rotation,scale);
}

CAFEL_NAMESPACE_END