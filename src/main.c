
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
#include "game_time.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef struct
{
	float x, y;

} Vec2;

typedef struct
{
	float x, y, z;

} Vec3;

typedef struct
{
	Vec3 position;
	float rotation;
	float scale;

} Transfrom;



typedef struct
{
	float r, g, b;

} Color;

typedef struct
{
	Vec3 vec;
	Color color;
	Vec2 uv;

} Vertex;

typedef struct MeshData
{
	Vertex* vertices;
	int verticesLen;
	int* indices;
	int indicesLen;
	bool avoidFreeMemory;

} MeshData;

typedef struct
{
	GLuint VBO;
	GLuint VAO;
	GLuint EBO;
	MeshData meshData;

} EBOBuffer;

typedef struct
{
	EBOBuffer renderBuffer;
	mat4x4 worldMatrix;

} RenderData;



Transfrom transform_default();

Transfrom transform_default()
{
	Transfrom tran;

	tran.scale = 1.0f;

	return tran;
}
void mesh_free(MeshData mesh);

#define M_PI 3.14f


void logger_create();
void logger_free();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

Vertex vertices[4] = {
	{{ 1.0f,  1.0f, 0},  {1,1,1},  { 1.0f, 1.0f  }},  // top right
	{{ 1.0f, -1.0f, 0},  {1,1,1},  { 1.0f, 0.0f  }}, // bottom right
	{{-1.0f, -1.0f, 0},  {1,1,1},  { 0.0f, 0.0f  }}, // bottom left
	{{-1.0f,  1.0f, 0},  {1,1,1},  { 0.0f, 1.0f  }},// top left 
};
unsigned int indices[6] = {  // note that we start from 0!
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

EBOBuffer create_element_array_buffer(MeshData mesh)
{
	int vertexBufferSize = sizeof(Vertex) * mesh.verticesLen;
	Vertex* vertices = mesh.vertices;
	int* indices = mesh.indices;
	int indices_size = sizeof(int) * mesh.indicesLen;

	EBOBuffer data;

	GLint VBO;
	GLuint VAO;
	GLuint EBO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);

	int posSize = sizeof(float) * 3;
	int colorSize = sizeof(float) * 3;
	int uvSize = sizeof(float)*2;
		
	int fullsize = sizeof(Vertex);

	//pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, fullsize,  (void*)0);
	glEnableVertexAttribArray(0);

	//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, fullsize, (void*)(posSize));
	glEnableVertexAttribArray(1);

	//uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, fullsize, (void*)(posSize + colorSize));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	data.VAO = VAO;
	data.VBO = VBO;
	data.EBO = EBO;
	data.meshData = mesh;
	return data;
}


void mesh_free(MeshData mesh)
{
	if (mesh.avoidFreeMemory)
		return;

	if (mesh.indices != NULL)
	{
		free(mesh.indices);
	}

	if (mesh.vertices != NULL)
	{
		free(mesh.vertices);
	}
}

MeshData create_circle_mesh(int segments)
{
	MeshData mesh;
	float l = 0.5f;
	float a = M_PI * 2 / segments;

	int arrayLen = segments * 3;

	Vertex* vertexArray = malloc(sizeof(Vertex) * arrayLen);
	int* indices = malloc(sizeof(int) * arrayLen);

	if (vertexArray == NULL)
	{
		exit(1);
		return mesh;
	}

	if (indices == NULL)
	{
		exit(1);
		return mesh;
	}

	Color color = { 1, 1, 0 };
	Vec3 center = { 0, 0, 0 };
	Vertex vert = { center, color };

	int offset = 0;
	for (int i = 0; i < arrayLen - 1; i += 3)
	{
		Vec3 currPos = { sin(a * offset) * l, cos(a * offset) * l, 0 };
		Vertex currVert = { currPos, color };

		Vec3 nextPos = { sin(a * (offset + 1)) * l, cos(a * (offset + 1)) * l, 0 };
		Vertex nextVert = { nextPos, color };

		vertexArray[i] = vert;
		vertexArray[i + 1] = currVert;
		vertexArray[i + 2] = nextVert;

		indices[i] = i;
		indices[i + 1] = i + 1;
		indices[i + 2] = i + 2;


		offset += 1;
	}
	mesh.vertices = vertexArray;
	mesh.verticesLen = arrayLen;
	mesh.indices = indices;
	mesh.indicesLen = arrayLen;

	return mesh;
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

	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(png_image_path, &width, &height, &nrChannels, 0);
	
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
	window = glfwCreateWindow(600, 600, "Snake GLFW", NULL, NULL);

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

	log_info("read vertex_shader");
	vertex_shader = shader_load_from_file(vert_shader_path, GL_VERTEX_SHADER);
	log_info("read fragment_shader");
	fragment_shader = shader_load_from_file(framg_shader_path, GL_FRAGMENT_SHADER);

	log_info("glCreateProgram");
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertex_shader);
	glAttachShader(shaderProgram, fragment_shader);
	glLinkProgram(shaderProgram);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	MeshData quad_mesh;

	quad_mesh.indices = indices;
	quad_mesh.avoidFreeMemory = true;
	quad_mesh.indicesLen = 6;
	quad_mesh.vertices = vertices;
	quad_mesh.verticesLen = 4;

	EBOBuffer quad_buffer = create_element_array_buffer(quad_mesh);

	MeshData circle_mesh;
	int seg = 30;
	int circle_size = seg * 3;
	circle_mesh = create_circle_mesh(seg);
	EBOBuffer circle_buffer = create_element_array_buffer(circle_mesh);
	int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
	unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");


	mat4x4 trans;
	mat4x4_identity(trans);
	Transfrom transform = transform_default();
	Transfrom transform2 = transform_default();

	RenderData quad_render;
	quad_render.renderBuffer = quad_buffer;
	RenderData circle_render;
	circle_render.renderBuffer = circle_buffer;

	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, trans);

	while (!glfwWindowShouldClose(window))
	{
		app_update_time();

		AppTime time = app_time_get();
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		float timeValue = glfwGetTime();

		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		
		glUseProgram(shaderProgram);
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, trans);

		mat4x4_identity(trans);

		transform.rotation += time.delta_time;

		log_info("%f glfw: %f", time.total_time, timeValue);
	
		mat4x4_translate(trans, 0.2f, 0, 0);
		mat4x4_rotate_Z(trans, trans, time.total_time);
		//mat4x4_scale(trans, trans, 0.1f);

		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(quad_buffer.VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_buffer.EBO);
		glDrawElements(GL_TRIANGLES, quad_mesh.indicesLen, GL_UNSIGNED_INT, 0);

		/*glBindVertexArray(circle_buffer.VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, circle_buffer.EBO);
		glDrawElements(GL_TRIANGLES, circle_mesh.indicesLen, GL_UNSIGNED_INT, 0);*/

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	log_info("destroy glfw window");

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &quad_buffer.VAO);
	glDeleteBuffers(1, &quad_buffer.VBO);
	glDeleteVertexArrays(1, &circle_buffer.VAO);
	glDeleteBuffers(1, &circle_buffer.VBO);
	glDeleteTextures(1, &texture);

	glfwDestroyWindow(window);

	glfwTerminate();

	logger_free();

	stbi_image_free(data);
	mesh_free(circle_mesh);
	mesh_free(quad_mesh);


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