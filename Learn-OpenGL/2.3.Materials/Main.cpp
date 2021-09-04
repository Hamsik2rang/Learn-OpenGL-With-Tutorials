#include "Shader.hpp"
#include "Camera.hpp"
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <string>

#include "stb_image.h"

constexpr unsigned int screenWidth = 1280;
constexpr unsigned int screenHeight = 720;

void framebufferSizeCallback(GLFWwindow*, int, int);
void cursurPosCallback(GLFWwindow*, double, double);
void scrollCallback(GLFWwindow*, double, double);
void processInput(GLFWwindow*);

Camera myCamera(glm::vec3(0.0f, 0.0f, 3.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Hamsik2rang's OpenGL window", NULL, NULL);

	if (!window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, cursurPosCallback);
	glfwSetScrollCallback(window, scrollCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialized GLAD" << std::endl;
		return -1;
	}

	Shader cubeShader("./VCube.vert", "./FCube.frag");
	Shader lightShader("./VLight.vert", "./FLight.frag");

	glEnable(GL_DEPTH_TEST);

	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	glm::vec3 cubePositions[5][4][6];
	for (int k = 0; k < 5; k++)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 6; j++)
			{
				cubePositions[k][i][j] = glm::vec3(-5.0f + (2 * j), 4.0f - (2 * i), -3.0f + (2 * k));
			}
		}
	}

	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glm::vec3 cubePos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 lightPos = glm::vec3(-5.0f, 0.0f, 0.0f);
	glm::vec3 lightColor = glm::vec3(1.0f);

	cubeShader.use();
	cubeShader.setValue("material.ambient", 1.0f, 0.5f, 0.31f);
	cubeShader.setValue("material.diffuse", 1.0f, 0.5f, 0.31f);
	cubeShader.setValue("material.specular", 0.5f, 0.5f, 0.5f);
	cubeShader.setValue("material.shininess", 32.0f);

	cubeShader.setValue("light.ambient", 0.2f, 0.2f, 0.2f);
	cubeShader.setValue("light.diffuse", 0.5f, 0.5f, 0.5f);
	cubeShader.setValue("light.specular", 1.0f, 1.0f, 1.0f);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float angle = 60.0f;
		//lightPos = glm::vec3(3 * cos(glm::radians(angle * (float)glfwGetTime())), lightPos.y, 3 * sin(glm::radians(angle) * (float)glfwGetTime()));
		lightPos = glm::vec3(-5 * (float)cos(float(glfwGetTime())), -5 * (float)sin(float(glfwGetTime())), 0.0f);
		for (int k = 0; k < 5; k++)
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 6; j++)
				{
					cubeShader.use();
					cubeShader.setValue("lightPos", lightPos);
					cubeShader.setValue("viewPos", myCamera.position());

					glm::mat4 proj = glm::perspective(glm::radians(myCamera.fov()), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
					glm::mat4 view = glm::lookAt(myCamera.position(), myCamera.position() + myCamera.forward(), myCamera.up());
					glm::mat4 model = glm::mat4(1.0f);
					//model = glm::rotate(model, 2.0f * (float)glfwGetTime(), glm::vec3(0.5f, 0.2f, 0.3f));
					model = glm::translate(model, cubePositions[k][i][j]);

					cubeShader.setValue("projection", proj);
					cubeShader.setValue("view", view);
					cubeShader.setValue("model", model);

					glBindVertexArray(VAO);
					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
			}
		}
		
		

		glm::mat4 proj = glm::perspective(glm::radians(myCamera.fov()), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(myCamera.position(), myCamera.position() + myCamera.forward(), myCamera.up());
		glm::mat4 model = glm::translate(glm::mat4(1.0f), lightPos);
		model = glm::scale(model, glm::vec3(0.2f));

		lightShader.use();
		lightShader.setValue("projection", proj);
		lightShader.setValue("view", view);
		lightShader.setValue("model", model);
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		myCamera.processKeyboard(CameraMovement::FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		myCamera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		myCamera.processKeyboard(CameraMovement::LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		myCamera.processKeyboard(CameraMovement::RIGHT, deltaTime);
	}
}

void cursurPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	static float lastX = 0.0f, lastY = 0.0f;
	static bool isFirstMove = true;
	if (isFirstMove)
	{
		lastX = xpos;
		lastY = ypos;
		isFirstMove = false;
	}

	float xoffset = static_cast<float>(xpos) - lastX;
	float yoffset = static_cast<float>(ypos) - lastY;
	lastX = xpos;
	lastY = ypos;

	myCamera.processMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	myCamera.processMouseScroll((float)yoffset);
}