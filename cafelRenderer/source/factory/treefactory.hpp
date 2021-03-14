#pragma once

#if !defined(_CAFEL_TREE_FACTORY_HPP_)
#define _CAFEL_TREE_FACTORY_HPP_

#include <cafel/system/factory.h>
#include <cafel/components/tree.h>

CAFEL_NAMESPACE_BEGIN

class TreeFactory : public Factory
{
public:
	virtual std::shared_ptr<TreeNodeRange> getNodeRange(int type, const AABB &aabb) = 0;
	virtual std::shared_ptr<TreeNode> getNode(std::vector<std::shared_ptr<ObjectApi>>& objects_, TreeNode *parent_) = 0;
	virtual std::shared_ptr<Tree> getTree() = 0;

	virtual ~TreeFactory() {}
};

CAFEL_NAMESPACE_END

#endif 