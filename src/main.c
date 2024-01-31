
//TODO: updaet snake to window version
//#include <stdio.h>
//#include <time.h>
//#include "header.h"
//
//int main()
//{
//	srand((unsigned int)time(NULL));
//
//	game_init();
//
//	move_snake_t = move_tick_time;
//	while (exit_app == 0)
//	{
//		input_read();
//		app_update_time();
//
//		if (pause == 1)
//			continue;
//
//		game_update();
//	}
//
//	delete_game_data();
//	return 0;
//}

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "linmath.h"

#include <stdlib.h>
#include "log.h";
#include <math.h>
#include <stdio.h>


typedef struct BufferAndArray
{
	GLuint VBO;
	GLuint VAO;
	unsigned int EBO;
} BufferAndArray;

typedef struct Vec2
{
	float x, y;
} Vec2;

typedef struct Color
{
	float r, g, b;
} Color;

typedef struct Vertex
{
	Vec2 vec;
	//Color color;
} Vertex;

#define M_PI 3.14f

char* read_file(char* path);

void logger_create();
void logger_free();

GLuint shader_load_from_text(char* shader_text, GLenum shader_type);
GLuint shader_load_from_file(char* shader_path, GLenum shader_type);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

//Vec2 posBuff[6] =
//{
//	{ -1, -1 },
//	{1, -1.f},
//	{1, 1 },
//	{-1, -1},
//	{-1, 1.f},
//	{1, 1},
//};
//
//Color colorBuff[6] =
//{
//	{ 0.f, 0.f, 1.f },
//	{ 0.f, 0.f, 1.f },
//	{ 0.f, 0.f, 1.f },
//	{ 0.f, 1.f, 0.f },
//	{ 0.f, 1.f, 0.f },
//	{ 0.f, 1.f, 0.f },
//};

Vertex vertices [4] = {
	{ 0.5f,  0.5f },  // top right
	{ 0.5f, -0.5f},  // bottom right
	{-0.5f, -0.5f},  // bottom left
	{-0.5f,  0.5f},  // top left 
};
unsigned int indices[] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

float vertices2[] = {
		-0.1f, -0.1f, 0.0f, // left  
		 0.1f, -0.1f, 0.0f, // right 
		 0.0f,  0.1f, 0.0f  // top   
};
//Vertex vertices[3] =
//{
//    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
//    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
//    {   0.f,  0.6f, 0.f, 0.f, 1.f }
//};

GLuint vertex2D_VBO;

static void error_callback(int error, const char* description)
{
	log_info("Error: %s", description);
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void read_png_file(char* file_path)
{
	FILE* fp = fopen(file_path, "rb");

	char header[8];

	if (!fp) {

		log_error("[read_png_file] File %s could not be opened for reading", file_path);
		fprintf(stderr, "[read_png_file] File %s could not be opened for reading", file_path);
		return NULL;
	}

	fread(header, 1, 8, fp);

	int result = png_sig_cmp(header);
	log_info("is png %i", result);

	fclose(fp);
}

int png_sig_cmp(char* header)
{
	const char png_first_bytes[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };

	for (size_t i = 0; i < 8; i++)
	{
		if (png_first_bytes[i] != header[i])
		{
			return 0;
		}
	}

	return 1;
}

BufferAndArray create_buffer(int bufferSize, Vertex* vertices, int* indices, int indices_size)
{
	BufferAndArray data;
	GLint VBO;
	GLuint VAO;
	GLuint EBO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	data.VAO = VAO;
	data.VBO = VBO;
	data.EBO = EBO;

	return data;
}

Vertex* create_circle_vertex(int segments)
{
	float l = 0.5f;
	float a = M_PI * 2 / segments;
	Vertex* vertexArray = malloc(sizeof(Vertex) * (segments * 3));

	if (vertexArray == NULL)
	{
		exit(1);
		return NULL;
	}

	Vec2 center = { 0, 0 };
	Color color = { 1, 0, 0 };
	Vertex vert = { center };

	int offset = 0;
	for (int i = 0; i < segments * 3 - 1; i += 3)
	{
		Vec2 currPos = { sin(a * offset) * l, cos(a * offset) * l };
		Vertex currVert = { currPos };

		Vec2 nextPos = { sin(a * (offset + 1)) * l, cos(a * (offset + 1)) * l };
		Vertex nextVert = { nextPos };

		vertexArray[i] = vert;
		vertexArray[i + 1] = currVert;
		vertexArray[i + 2] = nextVert;

		offset += 1;
	}

	return vertexArray;
}
Vertex* create_square()
{
	Vertex* vertBuffer = malloc(sizeof(Vertex) * 6);
	Vertex vert0 = { -1, -1 };
	Vertex vert1 = { 1, -1.f };
	Vertex vert2 = { 1,  1 };
	Vertex vert3 = { -1, -1 };
	Vertex vert4 = { -1, 1.f };
	Vertex vert5 = { 1, 1 };

	if (vertBuffer == NULL)
	{
		exit(1);
		return NULL;
	}

	vertBuffer[0] = vert0;
	vertBuffer[1] = vert1;
	vertBuffer[2] = vert2;
	vertBuffer[3] = vert3;
	vertBuffer[4] = vert4;
	vertBuffer[5] = vert5;

	return vertBuffer;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main(void)
{
	logger_create();
	log_info("logger inited");

	GLFWwindow* window;
	GLuint vertex_shader, fragment_shader, program;

	const char* vert_shader_path = "./resources/shaders/vert_shader.vert";
	const char* framg_shader_path = "./resources/shaders/fragm_shader.frag";
	const char* tga_image_path = "./resources/dwsample-tga-640.tga";
	const char* png_image_path = "./resources/test.png";

	read_png_file(png_image_path);

	glfwSetErrorCallback(error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (!glfwInit())
	{
		log_error("glfwInit failed!");
		exit(EXIT_FAILURE);
	}

	log_info("glfwCreateWindow");
	window = glfwCreateWindow(640, 480, "Snake GLFW", NULL, NULL);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// NOTE: OpenGL error checks have been omitted for brevity

	vertex_shader = shader_load_from_file(vert_shader_path, GL_VERTEX_SHADER);
	fragment_shader = shader_load_from_file(framg_shader_path, GL_FRAGMENT_SHADER);

	log_info("glCreateProgram");
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	
	Vertex* circle_vert = NULL;
	log_info("create_square");
	//square_vert = create_square();
	BufferAndArray vertex_buffer = create_buffer(sizeof(Vertex) * 4, vertices, indices, sizeof(indices));

	int seg = 30;
	int circle_size = seg * 3;
	circle_vert = create_circle_vertex(seg);

	//BufferAndArray vertex_buffer_circle = create_buffer(sizeof(Vertex) * circle_size, circle_vert);

	log_info("create_buffer");


	//vertex_buffer2 = create_buffer(sizeof(vertices2), vertices2);

	while (!glfwWindowShouldClose(window))
	{
		float vratio;
		float hratio;

		int width, height;
		mat4x4 m, p, mvp;

		glfwGetFramebufferSize(window, &width, &height);

		vratio = width / (float)height;
		hratio = height / (float)width;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program);

		glBindVertexArray(vertex_buffer.VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer.EBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		/*glBindVertexArray(vertex_buffer_circle.VAO);
		glDrawArrays(GL_TRIANGLES, 0, circle_size);*/


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	log_info("destroy glfw window");

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(program);
	glDeleteVertexArrays(1, &vertex_buffer.VAO);
	//glDeleteVertexArrays(1, &vertex_buffer_circle.VAO);
	//glDeleteBuffers(1, &vertex_buffer_circle.VBO);
	glDeleteBuffers(1, &vertex_buffer.VBO);
	

	glfwDestroyWindow(window);

	glfwTerminate();

	logger_free();
	free(circle_vert);
	//system("pause");
	exit(EXIT_SUCCESS);
}

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


FILE* fptr;

const char* log_file_name = "log.txt";
const char* file_mode = "w";

void logger_create()
{
	fptr = fopen(log_file_name, file_mode);

	if (fptr == NULL)
	{

		return;
	}
	log_info("log linked to file: %s", log_file_name);
	log_add_fp(fptr, 0);
}

void logger_free()
{
	if (fptr == NULL)
		return;

	fclose(fptr);
}