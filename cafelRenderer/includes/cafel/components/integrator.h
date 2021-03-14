#pragma once

#if !defined(_CAFEL_INTEGRATOR_H_)
#define _CAFEL_INTEGRATOR_H_

#include <cafel/components/tree.h>
#include <cstdlib>

CAFEL_NAMESPACE_BEGIN

class Integrator : public CAFEL_ELEMENT
{
public:
	Integrator(SceneNode *node) :Element(node) 
	{
		std::string depth = node->getParam("maxDepth");
		maxDepth = atoi(depth.c_str());
	}
	Integrator(unsigned int maxDepth_ = 20) { maxDepth = maxDepth_; }

	//������������Ϊ���麯������ʹ���޷���ʵ����
	virtual ~Integrator() {}

	virtual Color radiance(const Ray &r, std::shared_ptr<Tree> tree) = 0;

protected:
	unsigned int maxDepth;
};

CAFEL_NAMESPACE_END

#endif 
