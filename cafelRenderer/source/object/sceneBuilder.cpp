#include <object/sceneBuilder.hpp>
#include <cafel/object/mdlHelper.h>
#include <factory/mainfactory.hpp>

CAFEL_NAMESPACE_BEGIN

SceneBuilder::SceneBuilder(SceneNode *node) :SceneBuilderApi(node)
{
	SceneNode *root = node;

	SceneNode *child = root->getChildFirst("camera");
	camera = std::shared_ptr<CameraApi>(MainFactory::getCamera(child));

	child = root->getChildFirst("integrator");
	integrator = std::shared_ptr<Integrator>(static_cast<Integrator*>(MainFactory::get(child)));

	if (root->hasChild("mdl"))
	{
		child = root->getChildFirst("mdl");
		if (child != nullptr)
			MdlHelperApi::get()->init(child);
	}

	auto begin = root->getChildBegin("object");
	auto end = root->getChildEnd("object");
	std::vector<std::shared_ptr<ObjectApi>> objects;
	for (auto iter = begin; iter != end; ++iter)
	{
		objects.push_back(std::shared_ptr<ObjectApi>(static_cast<ObjectApi*>(MainFactory::get(iter->second))));
	}

	TreeFactory *treeFactory = MainFactory::getTreeFactory(TreeType::KD);
	tree = treeFactory->getTree();
	tree->buildTree(objects);
}

CAFEL_NAMESPACE_END