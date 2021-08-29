#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class Shader
{
private:
	unsigned int id;

public:
	Shader(const char* vpath, const char* fpath)
	{
		std::string vcode, fcode;
		std::ifstream vShaderFile, fShaderFile;

		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			vShaderFile.open(vpath);
			fShaderFile.open(fpath);
			std::stringstream vShaderStream, fShaderStream;

			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vcode = vShaderStream.str();
			fcode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cerr << "ERROR::SHDER::FILE_NOT_SUCCESSFULLY_READ::" << e.what() << std::endl;
		}
		const char* vShaderCode = vcode.c_str();
		const char* fShaderCode = fcode.c_str();

		unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkStatus(vertex, GL_COMPILE_STATUS);

		unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkStatus(fragment, GL_COMPILE_STATUS);

		id = glCreateProgram();
		glAttachShader(id, vertex);
		glAttachShader(id, fragment);
		glLinkProgram(id);
		checkStatus(id, GL_LINK_STATUS);

		glDeleteShader(vertex);
		glDeleteShader(fragment);		
	}
	void checkStatus(unsigned int program, unsigned int statusCode);

	void use() { glUseProgram(id); }
	void setValue(const std::string& name, bool value) const { glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value); }
	void setValue(const std::string& name, int value) const { glUniform1i(glGetUniformLocation(id, name.c_str()), value); }
	void setValue(const std::string& name, float value) const { glUniform1f(glGetUniformLocation(id, name.c_str()), value); }
	void setValue(const std::string& name, glm::mat4 matrix) const { 
		glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix)); 
	}

	unsigned int getID() const { return id; }
};

void Shader::checkStatus(unsigned int program, unsigned int statusCode)
{
	int success;
	char infoLog[512]{ '\0' };
	glGetShaderiv(program, statusCode, &success);
	if (!success)
	{
		glGetShaderInfoLog(program, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}