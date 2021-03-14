#include <trees/kd/kdtree.hpp>
#include <factory/mainfactory.hpp>

CAFEL_NAMESPACE_BEGIN

void KdTree::buildTree(std::vector<std::shared_ptr<ObjectApi>> &objects)
{
	std::shared_ptr<TreeFactory> fac(MainFactory::getTreeFactory(TreeType::KD));
	root = fac->getNode(objects, nullptr);
}

CAFEL_NAMESPACE_END