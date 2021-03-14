#pragma once

#if !defined(_CAFEL_GL_SHADER_HPP_)
#define _CAFEL_GL_SHADER_HPP_

#include <cafel/basic/vector.hpp>
#include <cafel/basic/matrix.hpp>
#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>

CAFEL_NAMESPACE_BEGIN

class GLShader
{
public:
	unsigned int ID;

	unsigned int VAO = -1;
	unsigned int VBO = -1;
	unsigned int EBO = -1;
	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------
	GLShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
	{
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			// if geometry shader path is present, also load a geometry shader
			if (geometryPath != nullptr)
			{
				gShaderFile.open(geometryPath);
				std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}
		}
		catch (std::ifstream::failure e)
		{
			printf("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n");
		}

		compile(vertexCode, fragmentCode, geometryCode);
	}

	GLShader(std::string vertexCode, std::string fragmentCode, std::string geometryCode = "")
	{
		compile(vertexCode, fragmentCode, geometryCode);
	}

	// activate the shader
	// ------------------------------------------------------------------------
	void use()
	{
		glUseProgram(ID);
	}
	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setVec2(const std::string &name, const dVector2 &value) const
	{
		float a[2];
		for (int i = 0; i < 2; i++)
		{
			a[i] = value[i];
		}
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &a[0]);
	}

	void setVec2(const std::string &name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	// ------------------------------------------------------------------------
	void setVec3(const std::string &name, const dVector &value) const
	{
		float a[3];
		for (int i = 0; i < 3; i++)
		{
			a[i] = value[i];
		}
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &a[0]);
	}

	void setVec3(const std::string &name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(const std::string &name, const dVector4 &value) const
	{
		float a[4];
		for (int i = 0; i < 4; i++)
		{
				a[i] = value[i];
		}
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &a[0]);
	}

	void setVec4(const std::string &name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void setMat2(const std::string &name, const dMatrix2 &mat) const
	{
		float a[2][2];
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
				a[i][j] = mat[i][j];
		}
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &a[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat3(const std::string &name, const dMatrix &mat) const
	{
		float a[3][3];
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
				a[i][j] = mat[i][j];
		}
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &a[0][0]);
	}
	// ------------------------------------------------------------------------

	void setMat4(const std::string &name, const dMatrix4 &mat) const
	{
		float a[4][4];
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				a[i][j] = mat[i][j];
		}
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &a[0][0]);
	}

private:
	void compile(std::string vertexCode, std::string fragmentCode, std::string geometryCode)
	{
		const char* vShaderCode = vertexCode.c_str();
		const char * fShaderCode = fragmentCode.c_str();
		const char * gShaderCode = nullptr;
		if (!geometryCode.empty())
		{
			gShaderCode = geometryCode.c_str();
		}
		// 2. compile shaders
		unsigned int vertex, fragment;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
		// if geometry shader is given, compile geometry shader
		unsigned int geometry;
		if (gShaderCode != nullptr)
		{
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			checkCompileErrors(geometry, "GEOMETRY");
		}
		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (gShaderCode != nullptr)
			glAttachShader(ID, geometry);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (gShaderCode != nullptr)
			glDeleteShader(geometry);
	}

	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	void checkCompileErrors(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				printf(("ERROR::SHADER_COMPILATION_ERROR of type: "+type+"\n"+infoLog+"\n -- --------------------------------------------------- -- \n").c_str());
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				printf(("ERROR::PROGRAM_LINKING_ERROR of type: "+type+"\n"+infoLog+"\n -- --------------------------------------------------- -- \n").c_str());
			}
		}
	}
};

CAFEL_NAMESPACE_END

#endif 