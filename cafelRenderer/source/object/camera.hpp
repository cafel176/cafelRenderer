#pragma once

#if !defined(_CAFEL_CAMERA_HPP_)
#define _CAFEL_CAMERA_HPP_

#include <cafel/object/camera.h>

CAFEL_NAMESPACE_BEGIN

class Camera : public CameraApi
{
public:
	Camera(SceneNode *node) :CameraApi(node) {}
	~Camera(){}

	void render(const std::shared_ptr<Integrator> integrator, const std::shared_ptr<Tree> tree, OUT_PROCESS out);

private:
	Color sample(const std::shared_ptr<Integrator> integrator, const std::shared_ptr<Tree> tree, int x, int y, int sx, int sy);
	Color subPixel(const std::shared_ptr<Integrator> integrator, const std::shared_ptr<Tree> tree, int x, int y);
};

CAFEL_NAMESPACE_END

#endif 