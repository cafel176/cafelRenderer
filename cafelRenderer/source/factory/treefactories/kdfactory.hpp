#pragma once

#if !defined(_CAFEL_KDFACTORY_HPP_)
#define _CAFEL_KDFACTORY_HPP_

#include <factory/treefactory.hpp>
#include <trees/kd/kdtree.hpp>

CAFEL_NAMESPACE_BEGIN

enum KdRangeType
{
	XYZ = 0
};

class KdFactory : public TreeFactory
{
public:
	std::shared_ptr<TreeNodeRange> getNodeRange(int type, const AABB &aabb)
	{
		TreeNodeRange* range;
		switch (type)
		{
		case KdRangeType::XYZ: range = new XYZSplit(aabb); break;
		default: range = new XYZSplit(aabb); break;
		}

		return std::shared_ptr<TreeNodeRange>(range);
	}

	std::shared_ptr<TreeNode> getNode(std::vector<std::shared_ptr<ObjectApi>>& objects_, TreeNode *parent_)
	{
		TreeNode* node = new KdNode(objects_, parent_);
		return std::shared_ptr<TreeNode>(node);
	}

	std::shared_ptr<Tree> getTree()
	{
		Tree* tree = new KdTree();
		return std::shared_ptr<Tree>(tree);
	}
};

CAFEL_NAMESPACE_END

#endif 