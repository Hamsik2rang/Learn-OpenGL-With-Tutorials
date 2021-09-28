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
	unsigned int _id;

	void checkCompileStatus(unsigned int, unsigned int);
	void checkLinkStatus(unsigned int);
public:
	Shader(const char*, const char*);
	
	void use();
	unsigned int id() const;

	void setValue(const std::string&, bool) const;
	void setValue(const std::string&, float) const;
	void setValue(const std::string&, int) const;
	void setValue(const std::string&, glm::mat2) const;
	void setValue(const std::string&, glm::mat3) const;
	void setValue(const std::string&, glm::mat4) const;
	void setValue(const std::string&, float, float, float) const;
	void setValue(const std::string&, glm::vec3) const;
};

Shader::Shader(const char* vpath, const char* fpath)
{
	std::ifstream vShaderFile, fShaderFile;
	std::string vcode, fcode;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vShaderFile.open(vpath);
		fShaderFile.open(fpath);

		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vcode = vShaderStream.str();
		fcode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cerr << e.what() << std::endl;
	}

	const char* vShaderCode = vcode.c_str();
	const char* fShaderCode = fcode.c_str();

	unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileStatus(vertex, GL_COMPILE_STATUS);

	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileStatus(fragment, GL_COMPILE_STATUS);

	_id = glCreateProgram();
	glAttachShader(_id, vertex);
	glAttachShader(_id, fragment);
	glLinkProgram(_id);
	checkLinkStatus(GL_LINK_STATUS);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use()
{
	glUseProgram(_id);
}

unsigned int Shader::id() const
{
	return _id;
}

void Shader::setValue(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(_id, name.c_str()), (int)value);
}

void Shader::setValue(const std::string& name, int value) const 
{ 
	glUniform1i(glGetUniformLocation(_id, name.c_str()), value);
}
void Shader::setValue(const std::string& name, float value) const 
{ 
	glUniform1f(glGetUniformLocation(_id, name.c_str()), value); 
}

void Shader::setValue(const std::string& name, glm::mat2 matrix) const 
{ 
	glUniformMatrix2fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix)); 
}

void Shader::setValue(const std::string& name, glm::mat3 matrix) const 
{
	glUniformMatrix3fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setValue(const std::string& name, glm::mat4 matrix) const 
{ 
	glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setValue(const std::string& name, float x, float y, float z) const 
{ 
	glUniform3f(glGetUniformLocation(_id, name.c_str()), x, y, z); 
}

void Shader::setValue(const std::string& name, glm::vec3 vec) const 
{ 
	glUniform3fv(glGetUniformLocation(_id, name.c_str()), 1, &vec[0]); 
}

void Shader::checkCompileStatus(unsigned int program, unsigned int statusCode)
{
	int success = 0;
	char infoLog[512]{ '\0' };
	glGetShaderiv(program, statusCode, &success);
	if (!success)
	{
		glGetShaderInfoLog(program, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}

void Shader::checkLinkStatus(unsigned int statusCode)
{
	int success = 0;
	char infoLog[512]{ '\0' };
	glGetProgramiv(_id, statusCode, &success);
	if (!success)
	{
		glGetProgramInfoLog(_id, GL_LINK_STATUS, NULL, infoLog);
		std::cerr << "ERROR::SHADER::LINKING_FAILED\n" << infoLog << std::endl;
	}
}