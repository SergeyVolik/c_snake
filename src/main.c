
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
#include "shader.h"
#include "image.h"

typedef struct EBOBuffer
{
	GLuint VBO;
	GLuint VAO;
	GLuint EBO;
} EBOBuffer;

typedef struct VAOBuffer
{
	GLuint VBO;
	GLuint VAO;
} VAOBuffer;

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
	Color color;
} Vertex;

#define M_PI 3.14f


void logger_create();
void logger_free();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);


Vertex vertices[4] = {
	{   { 0.5f,  0.5f }, {1,0,0}},  // top right
	{	{ 0.5f, -0.5f}, {1,0,0}}, // bottom right
	{	{-0.5f, -0.5f}, {0,0,1}}, // bottom left
	{	{-0.5f,  0.5f}, {0,1,0}}, // top left 
};
unsigned int indices[] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

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

VAOBuffer create_array_buffer(int bufferSize, Vertex* vertices)
{
	VAOBuffer data;

	GLint VBO;
	GLuint VAO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	data.VAO = VAO;
	data.VBO = VBO;

	return data;
}

EBOBuffer create_element_array_buffer(int bufferSize, Vertex* vertices, int* indices, int indices_size)
{
	EBOBuffer data;
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

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main(void)
{
	logger_create();
	log_info("logger inited");

	GLFWwindow* window;
	GLuint vertex_shader, fragment_shader, shaderProgram;

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
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertex_shader);
	glAttachShader(shaderProgram, fragment_shader);
	glLinkProgram(shaderProgram);


	Vertex* circle_vert = NULL;

	EBOBuffer vertex_buffer = create_element_array_buffer(sizeof(Vertex) * 4, vertices, indices, sizeof(indices));
	int seg = 30;
	int circle_size = seg * 3;
	//circle_vert = create_circle_vertex(seg);

	//VAOBuffer vertex_buffer_circle = create_array_buffer(sizeof(Vertex) * circle_size, circle_vert);

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
		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		glUseProgram(shaderProgram);
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		glBindVertexArray(vertex_buffer.VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer.EBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	/*	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_circle.VBO);
		glBindVertexArray(vertex_buffer_circle.VAO);
		glDrawBuffer(GL_TRIANGLES, 0, 6);*/


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	log_info("destroy glfw window");

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &vertex_buffer.VAO);	
	glDeleteBuffers(1, &vertex_buffer.VBO);
	//glDeleteVertexArrays(1, &vertex_buffer_circle.VAO);
	//glDeleteBuffers(1, &vertex_buffer_circle.VBO);

	glfwDestroyWindow(window);

	glfwTerminate();

	logger_free();
	free(circle_vert);
	//system("pause");
	exit(EXIT_SUCCESS);
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