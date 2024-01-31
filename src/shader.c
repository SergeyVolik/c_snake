
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "shader.h"
#include "file_helper.h"

GLuint shader_load_from_text(char* shader_text, GLenum shader_type)
{
	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &shader_text, NULL);
	glCompileShader(shader);

	GLuint ok;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

	if (!ok)
	{
		GLchar log[2000];
		glGetShaderInfoLog(shader, 2000, NULL, log);
		printf("%s\n", log);
	}

	return shader;
}

GLuint shader_load_from_file(char* shader_path, GLenum shader_type)
{
	char* shader_text = read_file(shader_path);
	GLuint shader = shader_load_from_text(shader_text, shader_type);
	log_info("free file text");
	free(shader_text);

	return shader;
}