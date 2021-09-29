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

#define DEBUG false
#define COL_STRIDE 5
#define ROW_STRIDE 3

struct Geometry
{
	float* buffer;
	unsigned int capacity;
	unsigned int size;

	Geometry() : capacity(120), size(0)
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

	void add(float* t)
	{
		if (size + 2 >= capacity)
		{
			resize();
		}
		buffer[size++] = *(t + 0);
		buffer[size++] = *(t + 1);
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

void partition(glm::vec3, glm::vec3, glm::vec3, float*, float*, float*, Geometry&, int);
glm::vec3 getNormalPos(glm::vec3, glm::vec3, float);
unsigned int loadTexture(const char*);

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

	glEnable(GL_DEPTH_TEST);
	auto textureID = loadTexture("./2k_sun.jpg");
	Shader shader("./vertex.vert", "./fragment.frag");

	float* vertices = new float[120]{
		// position			// texture
		0.0f, 0.5f, 0.0f,	0.125f, 1.0f,
		0.0f, 0.0f, 0.5f,	0.0f,	0.5f,
		0.5f, 0.0f, 0.0f,	0.25f,	0.5f,

		0.0f, 0.5f, 0.0f,	0.375f, 1.0f,
		0.5f, 0.0f, 0.0f,	0.25f,	0.5f,
		0.0f, 0.0f,-0.5f,	0.5f,	0.5f,

		0.0f, 0.5f, 0.0f,	0.625f, 1.0f,
		0.0f, 0.0f, -0.5f,	0.5f,	0.5f,
		-0.5f, 0.0f, 0.0f,	0.75f,	0.5f,

		0.0f, 0.5f, 0.0f,	0.875f, 1.0f,
		-0.5f, 0.0f, 0.0f,	0.75f,	0.5f,
		0.0f, 0.0f, 0.5f,	1.0f,	0.5f,

		0.0f, -0.5f, 0.0f,	0.125f, 0.0f,
		0.0f, 0.0f, 0.5f,	0.0f,	0.5f,
		0.5f, 0.0f, 0.0f,	0.25f,	0.5f,

		0.0f, -0.5f, 0.0f,	0.375f, 0.0f,
		0.5f, 0.0f, 0.0f,	0.25f,	0.5f,
		0.0f, 0.0f,-0.5f,	0.5f,	0.5f,

		0.0f, -0.5f, 0.0f,	0.625f, 0.0f,
		0.0f, 0.0f, -0.5f,	0.5f,	0.5f,
		-0.5f, 0.0f, 0.0f,	0.75f,	0.5f,

		0.0f, -0.5f, 0.0f,	0.875f, 0.0f,
		-0.5f, 0.0f, 0.0f,	0.75f,	0.5f,
		0.0f, 0.0f, 0.5f,	1.0f,	0.5f
	};
	int size = 120;

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
	for (int i = 0; i < size / 9; i++)
	{
		glm::vec3 side[3] = { glm::vec3(0.0f, 0.0f, 0.0f) };
		float texCoord[3][2] = { 0.0f };
		for (int j = 0; j < 3; j++)
		{
			side[j] = glm::vec3((vertices)[(i * ROW_STRIDE * COL_STRIDE) + (j * COL_STRIDE) + 0], (vertices)[(i * ROW_STRIDE * COL_STRIDE) + (j * COL_STRIDE) + 1], (vertices)[(i * ROW_STRIDE * COL_STRIDE) + (j * COL_STRIDE) + 2]);

			texCoord[j][0] = vertices[(i * ROW_STRIDE * COL_STRIDE) + (j * COL_STRIDE) + 3];
			texCoord[j][1] = vertices[(i * ROW_STRIDE * COL_STRIDE) + (j * COL_STRIDE) + 4];
		}
		partition(side[0], side[1], side[2], texCoord[0], texCoord[1], texCoord[2], geoBuffer, 3);
	}
	vertices = geoBuffer.buffer;
	size = geoBuffer.size;


#if DEBUG
	for (int i = 0; i < size / 3; i++)
	{
		std::cout << vertices[3 * i] << " " << vertices[3 * i + 1] << " " << vertices[3 * i + 2] << std::endl;
	}
#endif

	//assert(size % 5 == 0);

	unsigned int VAO, VBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), vertices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	shader.use();

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

unsigned int loadTexture(const char* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = 0;
		if (nrComponents == 1)
		{
			format = GL_RED;
		}
		else if (nrComponents == 3)
		{
			format = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cerr << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void partition(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float* t1, float* t2, float* t3, Geometry& geometryBuffer, int depth)
{
	if (depth > 0)
	{
		glm::vec3 n1 = (p1 + p2) / 2.0f;
		glm::vec3 n2 = (p1 + p3) / 2.0f;
		glm::vec3 n3 = (p2 + p3) / 2.0f;

		float nt1[2] = { (t1[0] + t2[0]) / 2.0f, (t1[1] + t2[1]) / 2.0f };
		float nt2[2] = { (t1[0] + t3[0]) / 2.0f, (t1[1] + t3[1]) / 2.0f };
		float nt3[2] = { (t2[0] + t3[0]) / 2.0f, (t2[1] + t3[1]) / 2.0f };


		partition(n1, n2, n3, nt1, nt2, nt3, geometryBuffer, depth - 1);
		partition(p1, n1, n2, t1, nt1, nt2, geometryBuffer, depth - 1);
		partition(n1, p2, n3, nt1, t2, nt3, geometryBuffer, depth - 1);
		partition(n2, n3, p3, nt2, nt3, t3, geometryBuffer, depth - 1);
	}
	else
	{
		glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);

		geometryBuffer.add(getNormalPos(p1, center, 0.8f));
		geometryBuffer.add(t1);
		geometryBuffer.add(getNormalPos(p2, center, 0.8f));
		geometryBuffer.add(t2);
		geometryBuffer.add(getNormalPos(p3, center, 0.8f));
		geometryBuffer.add(t3);
	}
}

glm::vec3 getNormalPos(glm::vec3 p, glm::vec3 center, float length)
{
	glm::vec3 norm = p - center;
	norm = glm::normalize(norm) * length;

	return norm;
}