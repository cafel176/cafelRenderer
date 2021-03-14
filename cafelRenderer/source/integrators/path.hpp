#pragma once

#if !defined(_CAFEL_PATH_HPP_)
#define _CAFEL_PATH_HPP_

#include <cafel/components/integrator.h>

CAFEL_NAMESPACE_BEGIN

class Path : public Integrator
{
public:
	Path(SceneNode *node) :Integrator(node) {}
	Path(unsigned int maxDepth = 10):Integrator(maxDepth){}

	Color radiance(const Ray &r, const std::shared_ptr<Tree> tree);

};

CAFEL_NAMESPACE_END

#endif 