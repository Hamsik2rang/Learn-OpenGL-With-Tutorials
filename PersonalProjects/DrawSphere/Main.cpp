#include "Camera.hpp"
#include "Shader.hpp"
#include "Constant.h"
#include "Random.hpp"

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "stb_image.h"

#include <iostream>
#include <string>
#include <cstring>
#include <cassert>
#include <vector>


struct Geometry
{
	float* buffer;
	unsigned int capacity;
	unsigned int size;

	Geometry() : capacity(72), size(0)
	{
		buffer = new float[capacity];
	}

	~Geometry()
	{
		delete[] buffer;
	}

	void add(glm::vec3 v)
	{
		if (size + 3 >= capacity)
		{
			resize();
		}
		buffer[size++] = v.x;
		buffer[size++] = v.y;
		buffer[size++] = v.z;
	}

	void resize()
	{
		capacity *= 4;
		auto nextBuffer = new float[capacity];
		memcpy(nextBuffer, buffer, sizeof(float) * size);
		delete[] buffer;
		buffer = nextBuffer;
	}
};

void framebufferSizeCallback(GLFWwindow*, int, int);
void cursorPosCallback(GLFWwindow*, double, double);
void scrollCallback(GLFWwindow*, double, double);
void processInput(GLFWwindow*);

void GeometryShader(float**, int&, unsigned int, unsigned int);
void partition(glm::vec3, glm::vec3, glm::vec3, Geometry&, int);
glm::vec3 getNormalPos(glm::vec3, glm::vec3, float);
//unsigned int loadTexture(const char*);

Hsrang::Camera camera(0.0f, 0.0f, 3.0f);


int main()
{
	// Initialize openGL and Make Window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(Hsrang::Constant::screenWidth, Hsrang::Constant::screenHeight, "Draw Sphere", nullptr, nullptr);

	assert(window);

	// Make context and Set Callbacks
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetScrollCallback(window, scrollCallback);

	assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

	//glEnable(GL_DEPTH_TEST);

	Shader shader("./vertex.vert", "./fragment.frag");

	float* vertices = new float[72]{
		0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.5f,
		0.5f, 0.0f, 0.0f,

		0.0f, 0.5f, 0.0f,
		0.5f, 0.0f, 0.0f,
		0.0f, 0.0f,-0.5f,

		0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, -0.5f,
		-0.5f, 0.0f, 0.0f,

		0.0f, 0.5f, 0.0f,
		-0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f,

		0.0f, -0.5f, 0.0f,
		0.0f, 0.0f, 0.5f,
		0.5f, 0.0f, 0.0f,

		0.0f, -0.5f, 0.0f,
		0.5f, 0.0f, 0.0f,
		0.0f, 0.0f,-0.5f,

		0.0f, -0.5f, 0.0f,
		0.0f, 0.0f, -0.5f,
		-0.5f, 0.0f, 0.0f,

		0.0f, -0.5f, 0.0f,
		-0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f,
	};
	int size = 72;

	std::vector<glm::vec3> spherePosition{
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f),
	};
	for (int i = 0; i < 690; i++)
	{
		spherePosition.push_back(Hsrang::Utility::getRandomPosition());
	}

	Geometry geoBuffer;
	glm::vec3 side[3] = { glm::vec3(0.0f, 0.0f, 0.0f) };
	for (int i = 0; i < size / 9; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			side[j] = glm::vec3((vertices)[(i * 9) + (j * 3) + 0], (vertices)[(i * 9) + (j * 3) + 1], (vertices)[(i * 9) + (j * 3) + 2]);
		}
		partition(side[0], side[1], side[2], geoBuffer, 4);
	}
	vertices = geoBuffer.buffer;
	size = geoBuffer.size;

	//for (int i = 0; i < size / 3; i++)
	//{
	//	std::cout << vertices[3 * i] << " " << vertices[3 * i + 1] << " " << vertices[3 * i + 2] << std::endl;
	//}

	unsigned int VAO, VBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), vertices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	shader.use();

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		glBindVertexArray(VAO);

		glm::mat4 view = glm::lookAt(camera.pos(), camera.pos() + camera.forward(), camera.up());
		glm::mat4 projection = glm::perspective(glm::radians(camera.fov()), Hsrang::Constant::screenWidth / Hsrang::Constant::screenHeight, 0.1f, 100.0f);
		
		shader.use();
		shader.setValue("view", view);
		shader.setValue("projection", projection);
		for (int i = 0; i < spherePosition.size(); i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, spherePosition[i]);
			float angle = 15.0f;
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(0.7f, 0.5f, 0.3f));
			shader.setValue("model", model);

			glDrawArrays(GL_TRIANGLES, 0, size);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();

	return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	static float lastTime = 0.0f;
	float curTime = (float)glfwGetTime();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	int forward = 0, right = 0;
	bool isMoved = false;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		forward = 1;
		isMoved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		forward = -1;
		isMoved = true;

	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		right = 1;
		isMoved = true;

	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		right = -1;
		isMoved = true;

	}
	if (isMoved)
	{
		camera.move(forward, right, curTime - lastTime);

	}
	lastTime = curTime;
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	camera.rotate(xpos, ypos);
}

void scrollCallback(GLFWwindow* window, double dx, double dy)
{
	camera.zoom(dy);
}

//unsigned int loadTexture(const char*);

void GeometryShader(float** vertices, int& size, unsigned int VAO, unsigned int VBO)
{
	static float branchTime = 3.0f;
	static float depth = 0;

	if ((float)glfwGetTime() > branchTime)
	{
		branchTime += 3.0f;
		depth++;

		Geometry geoBuffer;
		glm::vec3 side[3] = { glm::vec3(0.0f, 0.0f, 0.0f) };
		for (int i = 0; i < size / 9; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				side[j] = glm::vec3((*vertices)[(i * 9) + (j * 3) + 0], (*vertices)[(i * 9) + (j * 3) + 1], (*vertices)[(i * 9) + (j * 3) + 2]);
			}
			partition(side[0], side[1], side[2], geoBuffer, depth);
		}
		*vertices = geoBuffer.buffer;
		size = geoBuffer.size;
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, *vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
}

void partition(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, Geometry& geometryBuffer, int depth)
{
	if (depth > 0)
	{
		glm::vec3 n1 = (p1 + p2) / 2.0f;
		glm::vec3 n2 = (p1 + p3) / 2.0f;
		glm::vec3 n3 = (p2 + p3) / 2.0f;

		partition(n1, n2, n3, geometryBuffer, depth - 1);
		partition(p1, n1, n2, geometryBuffer, depth - 1);
		partition(n1, p2, n3, geometryBuffer, depth - 1);
		partition(n2, n3, p3, geometryBuffer, depth - 1);
	}
	else
	{
		glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
		geometryBuffer.add(getNormalPos(p1, center, 0.8f));
		geometryBuffer.add(getNormalPos(p2, center, 0.8f));
		geometryBuffer.add(getNormalPos(p3, center, 0.8f));
	}
}

glm::vec3 getNormalPos(glm::vec3 p, glm::vec3 center, float length)
{
	glm::vec3 norm = p - center;
	norm = glm::normalize(norm) * length;

	return norm;
}