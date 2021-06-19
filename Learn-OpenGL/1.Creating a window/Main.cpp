#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <iostream>
#include <Windows.h>

void framebufferSizeCallback(GLFWwindow*, int, int);
void processInput(GLFWwindow*);

int main()
{
	HWND hConsole = GetConsoleWindow();
	ShowWindow(hConsole, SW_HIDE);

	glfwInit();
	// Notice to compiler that we use major OpenGL version 3.x.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// and miner version using is x.3. in conclusion, we'll use version 3.3.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// use Core profile.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Hamsik2rang's first OpenGL window.", NULL, NULL);
	if (!window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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
		glfwSetWindowShouldClose(window, true);
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		// Change window's background.
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		framebufferSizeCallback(window, 480, 320);
	}
}