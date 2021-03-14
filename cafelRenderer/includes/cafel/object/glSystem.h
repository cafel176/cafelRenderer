#pragma once

#if !defined(_CAFEL_GL_SYSTEM_H_)
#define _CAFEL_GL_SYSTEM_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <thread>

#include <object/glShader.hpp>
#include <cafel/object/sceneBuilder.h>

CAFEL_NAMESPACE_BEGIN

class GLSystemApi : CAFEL_API
{
public:
	GLSystemApi(SceneBuilderApi* scene_, double scale):scene(scene_)
	{
		//初始化glfw
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
	//创建glfw窗口
		window = glfwCreateWindow(scene->getWidth()*scale, scene->getHeight()*scale, "openGL", NULL, NULL);
		if (window == NULL)
		{
			printf("Failed to create GLFW window\n");
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(window);

		//注册glad函数地址
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			printf("Failed to initialize GLAD\n");
			return;
		}

		// configure global opengl state
	// -----------------------------
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glViewport(0, 0, scene->getWidth()*scale, scene->getHeight()*scale);

		std::vector<std::shared_ptr<ObjectApi>> objects = scene->getObjects();
		for (unsigned int i = 0; i < objects.size(); i++)
			if (objects.at(i)->hasEmitter())
			{
				lightPos.push_back(objects.at(i)->getRealPosition());
			}

		for (unsigned int i = 0; i < objects.size(); i++)
		{			
			shaders.push_back(std::make_shared<GLShader>(
				objects.at(i)->getVertexShader(), 
				objects.at(i)->getFragmentShader(lightPos.size())
				));
		}
		
	}

	virtual ~GLSystemApi()
	{
		glfwSetWindowShouldClose(window, true);
	}

	HWND getHWND()
	{
		return glfwGetWin32Window(window);
	}

	std::thread* getRenderThread()
	{
		//将渲染移交线程前需要将当前上下文设为null
		glfwMakeContextCurrent(NULL);
		return new std::thread(&GLSystemApi::RenderProc,this);
	}

protected:
	GLFWwindow* window;
	SceneBuilderApi* scene;
	std::vector<dVector> lightPos;
	std::vector<std::shared_ptr<GLShader>> shaders;

	virtual void RenderProc() = 0;
};

CAFEL_NAMESPACE_END

#endif 