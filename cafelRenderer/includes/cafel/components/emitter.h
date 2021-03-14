#pragma once
#if !defined(_CAFEL_EMITTER_H_)
#define _CAFEL_EMITTER_H_

#include <cafel/system/element.h>
#include <cafel/basic/intersection.hpp>
#include <cafel/basic/color.hpp>
#include <cafel/utils/warp.hpp>
#include <cstdlib>
#include <sstream>

CAFEL_NAMESPACE_BEGIN

class Emitter : public CAFEL_ELEMENT
{
public:
	Emitter(SceneNode *node):Element(node) 
	{
		std::string color_ = node->getChildFirst("color")->getParam("value");
		std::vector<std::string> rgb = Warp::get()->split(color_, ",");
		color = Color(atof(rgb.at(0).c_str()), atof(rgb.at(1).c_str()), atof(rgb.at(2).c_str()));
	}
	Emitter(const Color &c_){ color = c_; }
	virtual ~Emitter() {}

	virtual Color eval(const Intersection &i)
	{
		return color;
	}
	virtual void generateCode(std::ostringstream &oss)
	{
		oss << "#version 330 core" << std::endl
			<< "out vec4 FragColor;" << std::endl
			<< std::endl
			<< "in VS_OUT {" << std::endl
			<< "    vec3 FragPos;" << std::endl
			<< "    vec3 Normal;" << std::endl
			<< "    vec2 TexCoords;" << std::endl
			<< "} fs_in;" << std::endl
			<< std::endl
			//<< "uniform vec3 rayDir;" << std::endl
			<< std::endl;
	}

protected:
	Color color = Color_one;
};

CAFEL_NAMESPACE_END

#endif 
