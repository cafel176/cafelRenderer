#pragma once

#if !defined(_CAFEL_XYZSPLIT_HPP_)
#define _CAFEL_XYZSPLIT_HPP_

#include <trees/kd/kdrange.hpp>

CAFEL_NAMESPACE_BEGIN

enum SplitType
{
	X = 0,
	Y = 1,
	Z = 2
};

class XYZSplit : public KdRange
{
public:
	XYZSplit(const AABB &aabb_, SplitType type_ = SplitType::X) : KdRange(aabb_),type(type_) {}
	~XYZSplit(){}

	SplitType getChildType();
	void setType(int t) { type = static_cast<SplitType>(t); }
	SplitType getType() { return type; }

private:
	SplitType type;
};

CAFEL_NAMESPACE_END

#endif 