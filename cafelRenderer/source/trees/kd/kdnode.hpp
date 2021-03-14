#pragma once

#if !defined(_CAFEL_KDNODE_HPP_)
#define _CAFEL_KDNODE_HPP_

#include <trees/kd/XYZSplit.hpp>

CAFEL_NAMESPACE_BEGIN

class KdNode : public TreeNode
{
public:
	KdNode(std::vector<std::shared_ptr<ObjectApi>> objects_, TreeNode *parent_);
	~KdNode() {}

	std::shared_ptr<TreeNode> getChild(int child) const{if (child == 0) return left;else return right;}
	std::shared_ptr<ObjectApi> intersect(double &t, const Ray &r) const;
	std::string tostring() const;

private:
	std::shared_ptr<TreeNode> left = nullptr;
	std::shared_ptr<TreeNode> right = nullptr;
};

CAFEL_NAMESPACE_END

#endif 