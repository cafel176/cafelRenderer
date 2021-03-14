#pragma once

#if !defined(_CAFEL_INTERSECTION_HPP_)
#define _CAFEL_INTERSECTION_HPP_

#include <cafel/basic/ray.hpp>

CAFEL_NAMESPACE_BEGIN

class Intersection : public Basic
{
public:
	Intersection(double t_, const Ray &r):t(t_),ray(r){}

	Intersection(
		double t_, const Ray &r, const dVector &n, 
		const dVector &ud, const dVector &vd, const dVector2 &uv_
	)
		:t(t_),ray(r), u_dir(ud),v_dir(vd),uv(uv_)
	{
		hitPoint = r.origin + r.direction * t;
		normal = n;
		normal.norm();
		normal_l = normal.dot(r.direction) < 0 ? normal : normal * -1;
		u_dir.norm();
		v_dir.norm();

		uv.x -= (int)uv.x;
		uv.y -= (int)uv.y;
		if (uv.x < 0)
			uv.x += 1;
		if (uv.y < 0)
			uv.y += 1;
	}

	Intersection(const Intersection &its)
		:t(its.t), ray(its.ray), hitPoint(its.hitPoint),
		normal(its.normal),normal_l(its.normal_l),
		out_ray(its.out_ray),uv(its.uv),
		u_dir(its.u_dir),v_dir(its.v_dir) {}

	Intersection& operator=(const Intersection& its)
	{
		t = its.t; ray = its.ray; out_ray = its.out_ray; 
		hitPoint = its.hitPoint; normal = its.normal; 
		normal_l = its.normal_l; uv = its.uv;
		u_dir = its.u_dir; v_dir = its.v_dir;
		return *this;
	}

	// 在intersect中得到
	double t;
	Ray ray;
	dVector hitPoint, normal, normal_l;
	dVector u_dir, v_dir;
	dVector2 uv;

	// 在bsdf sample中得到
	Ray out_ray;
};

CAFEL_NAMESPACE_END

#endif 