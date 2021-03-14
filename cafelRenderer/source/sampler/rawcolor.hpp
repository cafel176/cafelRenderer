#pragma once

#if !defined(_CAFEL_RAWCOLOR_HPP_)
#define _CAFEL_RAWCOLOR_HPP_

#include <cafel/components/sampler.h>
#include <cafel/utils/warp.hpp>
#include <cstdlib>

CAFEL_NAMESPACE_BEGIN

class RawColor : public Sampler
{
public:
	// 构造和析构函数
	RawColor(){}
	RawColor(const Color &c_) : color(c_) {}
	RawColor(SceneNode *node) : Sampler(node) 
	{
		std::string color_ = node->getChildFirst("color")->getParam("value");
		std::vector<std::string> rgb = Warp::get()->split(color_, ",");
		color = Color(atof(rgb.at(0).c_str()),atof(rgb.at(1).c_str()),atof(rgb.at(2).c_str()));
	}

	//继承函数
	Color eval(double u, double v);
	Color eval(const dVector2 &i);
	Color eval(const Intersection &i);

	// 独有函数
	void setColor(const Color &c_);
	Color getColor();
	std::string tostring() const { return "RawColor:("+std::string(color)+")"; }
	void generateCode(std::ostringstream &oss);

private:
	Color color = Color_one;
};

CAFEL_NAMESPACE_END

#endif 