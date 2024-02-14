#include <stdlib.h>

#include "window.h"
#include "log.h"

void GLFW_init()
{
	glfwSetErrorCallback(glfw_error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (!glfwInit())
	{
		log_error("glfwInit failed!");
		exit(EXIT_FAILURE);
	}
}

float lastX = 400, lastY = 300;
int firstMouse = 1;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = 0;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

}

GLFWwindow* window_create(char* title)
{
	GLFW_init();

	log_info("glfwCreateWindow");

	GLFWwindow* window = glfwCreateWindow(600, 600, title, NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);
	log_info("Window Created");
	return window;
}

void window_framebuffer_size_callback(GLFWwindow* window, GLFWframebuffersizefun callback)
{
	glfwSetFramebufferSizeCallback(window, callback);
}

void window_key_input_callback(GLFWwindow* window, GLFWkeyfun callback)
{
	glfwSetKeyCallback(window, callback);
}


void glfw_error_callback(int error, const char* description)
{
	log_info("Error: %s", description);
}