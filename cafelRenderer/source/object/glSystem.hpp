#pragma once

#if !defined(_CAFEL_GL_SYSTEM_HPP_)
#define _CAFEL_GL_SYSTEM_HPP_

#include <cafel/object/glSystem.h>

CAFEL_NAMESPACE_BEGIN



class GLSystem : public GLSystemApi
{
public:
	GLSystem(SceneBuilderApi* scene_,double scale):GLSystemApi(scene_,scale){}

protected:
	void RenderProc();

private:
	unsigned int loadTexture(Texture texture);
	void processInput(double& timer, double deltaTime, double currentTime);
	void renderTriangels(std::shared_ptr<GLShader> shader, std::shared_ptr<ObjectApi> object);
};

CAFEL_NAMESPACE_END

#endif 