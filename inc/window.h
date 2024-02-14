#ifndef WINDOW_H_SV
#define WINDOW_H_SV

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void GLFW_init();


void window_key_input_callback(GLFWwindow* window, GLFWkeyfun key_callback);
GLFWwindow* window_create(char* title);
void glfw_error_callback(int error, const char* description);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void window_framebuffer_size_callback(GLFWwindow* window, GLFWframebuffersizefun callback);

#endif