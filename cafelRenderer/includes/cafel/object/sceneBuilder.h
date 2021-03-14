#pragma once

#if !defined(_CAFEL_SCENE_BUILDER_H_)
#define _CAFEL_SCENE_BUILDER_H_

#include <cafel/system/sceneNode.h>
#include <cafel/object/camera.h>

CAFEL_NAMESPACE_BEGIN

class SceneBuilderApi: CAFEL_API
{
public:
	SceneBuilderApi(SceneNode *node){}

	virtual void render(OUT_PROCESS func = nullptr) = 0;
	virtual void saveImg(std::string filename = "") = 0;
	virtual unsigned char* output() = 0;
	virtual int getWidth() = 0;
	virtual int getHeight() = 0;
	virtual double getZoom() = 0;
	virtual dMatrix4 getViewMatrix() = 0;
	virtual void move(Camera_Movement *m, double deltaTime) = 0;
	virtual void rotate(Camera_Rotate *r, double deltaTime) = 0;
	virtual std::vector<std::shared_ptr<ObjectApi>> getObjects() = 0;
	virtual std::string getCameraInfo() const = 0;
	virtual dVector getCameraPos() const = 0;
	virtual void clearData() = 0;
};

CAFEL_NAMESPACE_END

#endif 