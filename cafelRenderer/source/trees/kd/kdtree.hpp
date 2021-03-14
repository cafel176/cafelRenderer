#pragma once

#if !defined(_CAFEL_KDTREE_HPP_)
#define _CAFEL_KDTREE_HPP_

#include <trees/kd/kdnode.hpp>

CAFEL_NAMESPACE_BEGIN

class KdTree : public Tree
{
public:
	void buildTree(std::vector<std::shared_ptr<ObjectApi>> &objects);
};

CAFEL_NAMESPACE_END

#endif 