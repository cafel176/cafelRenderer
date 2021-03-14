#pragma once

#if !defined(_CAFEL_SCENE_BUILDER_HPP_)
#define _CAFEL_SCENE_BUILDER_HPP_

#include <cafel/object/sceneBuilder.h>
#include <cafel/utils/imageOutput.hpp>

CAFEL_NAMESPACE_BEGIN

class SceneBuilder : public SceneBuilderApi
{
public:
	SceneBuilder(SceneNode *root);

	void render(OUT_PROCESS func)
	{
		camera->render(integrator, tree,func);
	}

	void saveImg(std::string filename = "")
	{
		std::string f = filename;
		if (f.empty())
			f = defaultFile;
		ImageOutput im_out(f);
		im_out.output(camera->output(), camera->getWidth(), camera->getHeight());
	}

	unsigned char* output() { return camera->output(); }
	int getWidth() { return camera->getWidth(); }
	int getHeight() { return camera->getHeight(); }
	double getZoom() { return camera->getZoom(); }
	dMatrix4 getViewMatrix() { return camera->getViewMatrix(); }
	void move(Camera_Movement *m, double deltaTime) { camera->move(m, deltaTime);}
	void rotate(Camera_Rotate *r, double deltaTime) { camera->rotate(r, deltaTime); }
	std::vector<std::shared_ptr<ObjectApi>> getObjects() { return tree->getObjects(); }
	std::string getCameraInfo() const { return camera->tostring(); }
	dVector getCameraPos() const { return camera->getPos(); }
	void clearData() { camera->clearData(); }
private:
	std::shared_ptr<Tree> tree;
	std::shared_ptr<Integrator> integrator;
	std::shared_ptr<CameraApi> camera;
};

CAFEL_NAMESPACE_END

#endif 