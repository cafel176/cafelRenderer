#pragma once

#if !defined(_CAFEL_SHAPE_H_)
#define _CAFEL_SHAPE_H_

#include <cafel/system/element.h>
#include <cafel/basic/ray.hpp>
#include <cafel/basic/intersection.hpp>
#include <cafel/utils/warp.hpp>
#include <cstdlib>
#include <sstream>
#include <cafel/basic/matrix.hpp>

CAFEL_NAMESPACE_BEGIN

class Shape : public CAFEL_ELEMENT
{
public:
	// 抽象类不能用初始化列表初始化
	Shape(SceneNode *node) :Element(node) 
	{
		if (node->hasChild("localPosition"))
		{
			std::string lp = node->getChildFirst("localPosition")->getParam("value");
			std::vector<std::string> vec = Warp::get()->split(lp, ",");
			localPosition = dVector(atof(vec.at(0).c_str()), atof(vec.at(1).c_str()), atof(vec.at(2).c_str()));
		}
		else
			localPosition = Vector_zero(double, 3);
		if (node->hasChild("localRotation"))
		{
			std::string lr = node->getChildFirst("localRotation")->getParam("value");
			std::vector<std::string> vec = Warp::get()->split(lr, ",");
			localRotation = dVector(atof(vec.at(0).c_str()), atof(vec.at(1).c_str()), atof(vec.at(2).c_str()));
		}
		else
			localRotation = Vector_zero(double, 3);
		if (node->hasChild("localScale"))
		{
			std::string ls = node->getChildFirst("localScale")->getParam("value");
			std::vector<std::string> vec = Warp::get()->split(ls, ",");
			localScale = dVector(atof(vec.at(0).c_str()), atof(vec.at(1).c_str()), atof(vec.at(2).c_str()));
		}
		else
			localScale = Vector_one(double,3);
	}
	Shape(const dVector &p_ = Vector_zero(double, 3), const dVector &r_ = Vector_zero(double, 3), const dVector &s_ = Vector_one(double, 3))
	{
		localPosition = p_; localRotation = r_; localScale = s_;
	}
	virtual ~Shape() { }

	// 返回射线的t，用以标记交点
	virtual Intersection intersect(const Ray &r, const dVector &position, const dVector &rotation, const dVector &scale) const = 0;

	virtual dVector getRealPosition(const dVector &position, const dVector &rotation) const
	{
		return localPosition + position;
	}
	virtual dVector getRealScale(const dVector &scale) const
	{
		return localScale * scale;
	}

	virtual dVector getMin(const dVector &position, const dVector &rotation, const dVector &scale) const = 0;
	virtual dVector getMax(const dVector &position, const dVector &rotation, const dVector &scale) const = 0;

	virtual std::string tostring() const = 0;

	const float* getTriangles(unsigned int &num, unsigned int &size) const
	{
		size = 8 * sizeof(float);
		num = vertices.size() / 8;

		return &vertices[0];
	}

	const unsigned int* getIndices(unsigned int &num, unsigned int &size) const
	{
		size = sizeof(unsigned int);
		num = indices.size();

		return &indices[0];
	}

	virtual void generateCode(std::ostringstream &oss)
	{		
		oss << "#version 330 core" << std::endl
			<< "layout (location = 0) in vec3 aPos;" << std::endl
			<< "layout (location = 1) in vec3 aNormal;" << std::endl
			<< "layout (location = 2) in vec2 aTexCoords;" << std::endl
			<< std::endl
			<< "out VS_OUT {" << std::endl
			<< "    vec3 FragPos;" << std::endl
			<< "    vec3 Normal;" << std::endl
			<< "    vec2 TexCoords;" << std::endl
			<< "} vs_out;" << std::endl
			<< std::endl
			<< "uniform mat4 projection;" << std::endl
			<< "uniform mat4 view;" << std::endl
			<< "uniform mat4 model;" << std::endl
			<< std::endl;
	}

protected:
	dVector localPosition;
	dVector localRotation;// 欧拉角
	dVector localScale;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;
};

CAFEL_NAMESPACE_END

#endif 
