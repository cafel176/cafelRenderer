#pragma once

#if !defined(_CAFEL_AABB_HPP_)
#define _CAFEL_AABB_HPP_

#include <cafel/basic/ray.hpp>
#include <cafel/utils/warp.hpp>

CAFEL_NAMESPACE_BEGIN

class AABB : public Basic
{
public:
	AABB(const dVector &min_ = Vector_zero(double,3), const dVector &max_ = Vector_zero(double,3)){ min = min_; max = max_; }
	AABB(const AABB &r) { min = r.min; max = r.max; }

	AABB& operator=(const AABB& other)
	{
		min = other.min; max = other.max;
		return *this;
	}

	bool intersect(const Ray &r,double &LargestMinT, double &LargestMaxT) const
	{	
		const dVector InvR = Vector_one(double,3) / r.direction;
		const dVector BottomT = InvR * (min - r.origin);
		const dVector TopT = InvR * (max - r.origin);
		const dVector MinT = Warp::get()->minVec(TopT, BottomT);
		const dVector MaxT = Warp::get()->maxVec(TopT, BottomT);

		LargestMinT = fmax(fmax(MinT.x, MinT.y), MinT.z);
		LargestMaxT = fmin(fmin(MaxT.x, MaxT.y), MaxT.z);
		return LargestMinT > LargestMaxT ? false : true;
	}

	bool inRange(const dVector& pos)
	{
		if ((pos.x >= min.x && pos.y >= min.y && pos.z >= min.z) &&
			(pos.x <= max.x && pos.y <= max.y && pos.z <= max.z))
			return true;
		else
			return false;
	}

	operator std::string() const
	{
		return "AABB(" +
			std::string("min:") + std::string(min) + " , " +
			std::string("max:") + std::string(max) +
		")";
	}

	dVector min, max;
};

CAFEL_NAMESPACE_END

#endif 