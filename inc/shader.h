#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

char* read_file(char* path);

GLuint shader_load_from_text(char* shader_text, GLenum shader_type);
GLuint shader_load_from_file(char* shader_path, GLenum shader_type);

#endif