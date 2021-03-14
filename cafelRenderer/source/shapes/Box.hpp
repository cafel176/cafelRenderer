#pragma once

#if !defined(_CAFEL_OBB_HPP_)
#define _CAFEL_OBB_HPP_

#include <cafel/components/shape.h>
#include <cstdlib>

CAFEL_NAMESPACE_BEGIN

class Box : public Shape
{
public:
	Box(SceneNode *node);

	Intersection intersect(const Ray &r, const dVector &position, const dVector &rotation, const dVector &scale) const;
	dVector getMin(const dVector &position, const dVector &rotation, const dVector &scale) const;
	dVector getMax(const dVector &position, const dVector &rotation, const dVector &scale) const;

	void setAxisX(dVector r_) { axisX = r_; axisX.norm(); }
	void setAxisY(dVector r_) { axisY = r_; axisY.norm(); }
	void setAxisZ(dVector r_) { axisZ = r_; axisZ.norm(); }
	void setExtend(dVector r_) { extend = r_; }
	std::string tostring() const { return "Box:(extend:" + std::string(extend) + ")"; }
	void generateCode(std::ostringstream &oss);

private:
	dVector axisX, axisY, axisZ, extend;
};

CAFEL_NAMESPACE_END

#endif 