#pragma once

#if !defined(_CAFEL_SPHERE_HPP_)
#define _CAFEL_SPHERE_HPP_

#include <cafel/components/shape.h>
#include <cstdlib>

CAFEL_NAMESPACE_BEGIN

class Sphere : public Shape
{
public:
	Sphere(SceneNode *node);

	Intersection intersect(const Ray &r, const dVector &position, const dVector &rotation, const dVector &scale) const;
	dVector getMin(const dVector &position, const dVector &rotation, const dVector &scale) const;
	dVector getMax(const dVector &position, const dVector &rotation, const dVector &scale) const;

	void setRadius(double r_) { radius = r_; }

	std::vector<float> getPoint(double u, double v);

	std::string tostring() const { return "Sphere:(radius:" + std::to_string(radius) + ")"; }
	void generateCode(std::ostringstream &oss);

private:
	double radius;

	int uStepsNum;
	int vStepsNum;
};

CAFEL_NAMESPACE_END

#endif 