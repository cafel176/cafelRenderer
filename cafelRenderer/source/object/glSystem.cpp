#include <object/glSystem.hpp>
#include <cafel/utils/warp.hpp>

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

CAFEL_NAMESPACE_BEGIN

void GLSystem::RenderProc()
{
	glfwMakeContextCurrent(window);
	double lastFrame = 0;
	double timer = -1; 
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		double currentFrame = glfwGetTime();
		double deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(timer, deltaTime,currentFrame);

		//刷新颜色缓冲和深度
		glClearColor(1.0f, 0.682f, 0.784f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		dMatrix4 projection = dMatrix4::perspective(Warp::get()->radians(scene->getZoom()), (double)scene->getWidth() / (double)scene->getHeight(), 0.1f, 1000.0f);
		dMatrix4 view = scene->getViewMatrix();

		std::vector<std::shared_ptr<ObjectApi>> objects = scene->getObjects();
		for (unsigned int i = 0; i < shaders.size(); i++)
		{		
			std::shared_ptr<GLShader> shader = shaders.at(i);
			shader->use();

			dMatrix4 model;
			model = dMatrix4::translate(model, objects.at(i)->getRealPosition());
			model = dMatrix4::scale(model, objects.at(i)->getRealScale());

			shader->setMat4("projection", projection);
			shader->setMat4("view", view);
			shader->setMat4("model", model);

			shader->setVec3("viewPos", scene->getCameraPos());
			for(int i=0;i<lightPos.size();i++)
				shader->setVec3("lightPos["+std::to_string(i)+"]", lightPos.at(i));
			renderTriangels(shader, objects.at(i));
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}

unsigned int GLSystem::loadTexture(Texture texture)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	
	unsigned char *data = texture.data;
	if (data)
	{
		GLenum format;
		if (texture.nrComponents == 1)
			format = GL_RED;
		else if (texture.nrComponents == 3)
			format = GL_RGB;
		else if (texture.nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		printf((std::string("Texture failed to load at path: ") + std::string(texture.path) + "\n").c_str());
	}

	return textureID;
}

void GLSystem::processInput(double& timer, double deltaTime, double currentTime)
{
	Camera_Movement m[3];
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m[0] = Camera_Movement::FORWARD;
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m[0] = Camera_Movement::BACKWARD;
	else
		m[0] = Camera_Movement::NONE;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m[1] = Camera_Movement::LEFT;
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m[1] = Camera_Movement::RIGHT;
	else
		m[1] = Camera_Movement::NONE;

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		m[2] = Camera_Movement::UP;
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		m[2] = Camera_Movement::DOWN;
	else
		m[2] = Camera_Movement::NONE;

	Camera_Rotate r[2];
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		r[0] = Camera_Rotate::Pitch_More;
	else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		r[0] = Camera_Rotate::Pitch_Less;
	else
		r[0] = Camera_Rotate::None;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		r[1] = Camera_Rotate::Yaw_More;
	else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		r[1] = Camera_Rotate::Yaw_Less;
	else
		r[1] = Camera_Rotate::None;

	Camera_Movement* arr = m;
	scene->move(arr, deltaTime);
	Camera_Rotate* arr2 = r;
	scene->rotate(arr2, deltaTime);

	if (currentTime - timer > 0.5)
	{
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			printf(scene->getCameraInfo().c_str());
			timer = currentTime;
		}			
	}
}

void GLSystem::renderTriangels(std::shared_ptr<GLShader> shader, std::shared_ptr<ObjectApi> object)
{
	unsigned int num1 = 0, size1 = 0, num2 = 0, size2 = 0;
	const float* vertices = object->getTriangles(num1, size1);
	const unsigned int* index = object->getIndices(num2, size2);

	// initialize (if necessary)
	if (shader->VAO == -1)
	{
		unsigned int VAO = 0;
		unsigned int VBO = 0;
		unsigned int EBO = 0;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, num1 * size1, vertices, GL_STREAM_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, num2 * size2, index, GL_STREAM_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, size1, (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, size1, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, size1, (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		shader->VAO = VAO;
		shader->VBO = VBO;
		shader->EBO = EBO;
	}

	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	std::vector<Texture> textures = object->getTextures();
	for(int i=0;i<textures.size();i++)
	{
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++); // transfer unsigned int to stream
		else if (name == "texture_normal")
			number = std::to_string(normalNr++); // transfer unsigned int to stream
		else if (name == "texture_height")
			number = std::to_string(heightNr++); // transfer unsigned int to stream

		if (textures[i].id == 0)
			object->setTextureID(loadTexture(textures[i]));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader->ID, (name + number).c_str()), 0);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glBindVertexArray(shader->VAO);
	glDrawElements(GL_TRIANGLES, num2, GL_UNSIGNED_INT, 0);
	glBindVertexArray(NULL);
}

CAFEL_NAMESPACE_END