
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "log.h"
#include "shader.h"

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
	free(shader_text);

	return shader;
}

char* read_file(char* path)
{
	FILE* fp;
	long lSize;
	char* buffer;

	fp = fopen(path, "rb");
	if (!fp)
	{
		log_error("file cant be open. file:  %s", path);
		perror(path);
		exit(1);
	}

	fseek(fp, 0L, SEEK_END);
	lSize = ftell(fp);
	rewind(fp);

	/* allocate memory for entire content */
	buffer = calloc(1, lSize + 1);

	if (!buffer)
	{
		log_error("memory alloc fails. file: %s", path);
		fclose(fp);
		fputs("memory alloc fails", stderr);
		exit(1);
	}
	/* copy the file into the buffer */
	if (1 != fread(buffer, lSize, 1, fp))
	{
		fclose(fp);
		free(buffer);
		log_error("entire read fails file: %s", path);
		fputs("entire read fails", stderr);
		exit(1);
	}

	/* do your work here, buffer is a string contains the whole text */

	fclose(fp);

	return buffer;
}