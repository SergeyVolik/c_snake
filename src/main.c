
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
#include "game_math.h";
#include <string.h>

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
	float r, g, b, a;

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
	unsigned int verticesLen;
	int* indices;
	unsigned int indicesLen;
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
	GLuint texture;

} RenderData;

#define M_PI 3.14f

void printf_color(Color* color);
void printf_mat4x4(mat4x4 mat);

Transfrom transform_default();
void mesh_free(MeshData mesh);
void logger_create();
void logger_free();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void glfw_error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
Color* create_color_texture(int width, int height, Color color);
void draw_renderer(const Transfrom* trans, const RenderData* renderData, const mat4x4* view, const mat4x4* proj, const GLuint modevMatPath);
void printf_color(Color* color);

void printf_color(Color* color)
{
	printf("Color(%f, %f, %f, %f)\n", color->r, color->g, color->b, color->a);
}

void printf_mat4x4(mat4x4 mat)
{
	printf("matrix:\n");
	for (size_t i = 0; i < 4; i++)
	{
		printf("[ ");
		for (size_t j = 0; j < 4; j++)
		{
			printf("%f ", mat[i][j]);

			if (j != 3)
			{
				printf(",");
			}
		}
		printf("]\n");
	}
}

Transfrom transform_default()
{
	Transfrom tran;

	Vec3 pos = { 0,0,0 };
	tran.position = pos;

	tran.scale = 1.0f;
	tran.rotation = 0;
	return tran;
}

Vertex vertices[4] = {
	{{ 1.0f,  1.0f, 0},  {1,1,1,1},  { 1.0f, 1.0f  }},  // top right
	{{ 1.0f, -1.0f, 0},  {1,1,1,1},  { 1.0f, 0.0f  }}, // bottom right
	{{-1.0f, -1.0f, 0},  {1,1,1,1},  { 0.0f, 0.0f  }}, // bottom left
	{{-1.0f,  1.0f, 0},  {1,1,1,1},  { 0.0f, 1.0f  }},// top left 
};

unsigned int indices[6] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

static void glfw_error_callback(int error, const char* description)
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

	EBOBuffer data;

	GLint VBO;
	GLuint VAO;
	GLuint EBO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);


	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertices, GL_STATIC_DRAW);

	if (mesh.indices != NULL)
	{
		int* indices = mesh.indices;
		int indices_size = sizeof(int) * mesh.indicesLen;

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
		data.EBO = EBO;
	}

	int posSize = sizeof(float) * 3;
	int colorSize = sizeof(float) * 4;
	int uvSize = sizeof(float) * 2;

	int fullsize = sizeof(Vertex);

	//pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, fullsize, (void*)0);
	glEnableVertexAttribArray(0);

	//color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, fullsize, (void*)(posSize));
	glEnableVertexAttribArray(1);

	//uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, fullsize, (void*)(posSize + colorSize));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	data.VAO = VAO;
	data.VBO = VBO;

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

	Vertex* vertexArray = (Vertex*)malloc(sizeof(Vertex) * arrayLen);
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

	Color color = { 1, 1, 1, 1 };
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
	/*mesh.indices = indices;
	mesh.indicesLen = arrayLen;*/
	mesh.indices = 0;
	mesh.indicesLen = 0;
	return mesh;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

Color* create_color_texture(int width, int height, Color color)
{
	Color* textureColorData = (Color*)malloc(sizeof(Color) * width * height);

	if (textureColorData == NULL)
		return NULL;

	for (size_t i = 0; i < height; i++)
	{
		for (size_t j = 0; j < width; j++)
		{
			size_t index = i * height + j;

			textureColorData[index] = color;
		}
	}

	return textureColorData;
}

void draw_renderer(const Transfrom* trans, const RenderData* renderData, const mat4x4* view, const mat4x4* proj, const GLuint modevMatPath)
{
	mat4x4 modelMat;

	mat4x4_translate_vec3(modelMat, &trans->position);
	mat4x4_scale(modelMat, modelMat, trans->scale);
	mat4x4_rotate_Z(modelMat, modelMat, trans->rotation);

	glUniformMatrix4fv(modevMatPath, 1, GL_FALSE, (const GLfloat*)modelMat);

	glBindTexture(GL_TEXTURE_2D, renderData->texture);

	glBindVertexArray(renderData->renderBuffer.VAO);

	if (renderData->renderBuffer.EBO != NULL)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderData->renderBuffer.EBO);
		glDrawElements(GL_TRIANGLES, renderData->renderBuffer.meshData.indicesLen, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, renderData->renderBuffer.VBO);
		glDrawArrays(GL_TRIANGLES, 0, renderData->renderBuffer.meshData.verticesLen);
	}

}

void init_GLFW();
void init_GLFW()
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

GLFWwindow* window_create(char* title)
{
	log_info("glfwCreateWindow");

	GLFWwindow* window = glfwCreateWindow(600, 600, title, NULL, NULL);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(0);
	log_info("Window Created");
	return window;
}

void init_OpenGL();
void init_OpenGL()
{
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void load_resources();

void load_resources()
{

}

int targetFPS = 144;

const char* vert_shader_path = "./resources/shaders/vert_shader.vert";
const char* framg_shader_path = "./resources/shaders/fragm_shader.frag";
const char* tga_image_path = "./resources/dwsample-tga-640.tga";
const char* png_image_path = "./resources/test.png";

int main()
{
	logger_create();
	log_info("logger inited");

	char app_name[] = "Snake GLFW ( %i )";

	init_GLFW();
	GLFWwindow* window = window_create(app_name);

	log_info("init_OpenGL");
	init_OpenGL();
	log_info("load_resources");

	load_resources();

	stbi_set_flip_vertically_on_load(true);

	GLuint vertex_shader, fragment_shader, shaderProgram;

	int width, height, nrChannels;

	unsigned char* image_data = stbi_load(png_image_path, &width, &height, &nrChannels, STBI_rgb_alpha);

	int defTextWidth = 10;
	int defTextHeigh = 10;

	Color defaultColor = { 1,1,1,1 };

	Color* defaultTextureData = create_color_texture(defTextWidth, defTextHeigh, defaultColor);
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

	GLuint png_g_texture;
	glGenTextures(1, &png_g_texture);
	glBindTexture(GL_TEXTURE_2D, png_g_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	GLuint g_texture;
	glGenTextures(1, &g_texture);
	glBindTexture(GL_TEXTURE_2D, g_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, defTextWidth, defTextHeigh, 0, GL_RGBA, GL_FLOAT, defaultTextureData);
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

	GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

	Transfrom transform = transform_default();

	transform.position.x = 0.2f;

	Transfrom transformCircle = transform_default();

	RenderData quad_render;
	quad_render.renderBuffer = quad_buffer;
	quad_render.texture = png_g_texture;
	RenderData circle_render;
	circle_render.renderBuffer = circle_buffer;
	circle_render.texture = g_texture;

	char result[100] = "";
	sprintf(result, app_name, 0);
	while (!glfwWindowShouldClose(window))
	{
		app_update_time();
		AppTime time = app_time_get();
		app_update_fps(time.delta_time);

		glfwGetFramebufferSize(window, &width, &height);

		sprintf(result, app_name, app_fps());
		glfwSetWindowTitle(window, result);

		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		mat4x4 viewMat, projMat;

		float otrhoFacotr = 2.0f;

		float left = -1.0f * otrhoFacotr;
		float right = 1.0f * otrhoFacotr;
		float bottom = -1.0f * otrhoFacotr;
		float top = 1.0f * otrhoFacotr;

		mat4x4_ortho(projMat, left, right, bottom, top, -0.01f, 100.0f);
		mat4x4_translate(viewMat, 0, 0, 0);

		glUseProgram(shaderProgram);

		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (const GLfloat*)projMat);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (const GLfloat*)viewMat);

		transform.scale = 1 + (1 * time.total_time);
		transform.rotation = angle_to_radians(180);

		draw_renderer(&transform, &quad_render, &viewMat, &projMat, modelLoc);
		draw_renderer(&transformCircle, &circle_render, &viewMat, &projMat, modelLoc);

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
	glDeleteTextures(1, &png_g_texture);

	glfwDestroyWindow(window);

	glfwTerminate();

	logger_free();

	free(defaultTextureData);
	stbi_image_free(image_data);
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