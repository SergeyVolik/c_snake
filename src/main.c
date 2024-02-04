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
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "flecs.h"
#include "linmath.h"
#include "log.h"

#include "shader.h"
#include "image.h" 
#include "game_time.h"
#include "game_math.h"
#include "rendering.h"
#include "LocalTransform.h"
#include "camera.h"

void logger_init();
void logger_free();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void glfw_error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void GLFW_init();
void draw_renderer(const LocalTransfrom* trans, const RenderData* renderData, const mat4x4* view, const mat4x4* proj, const GLuint modevMatPath);
void init_OpenGL();
GLFWwindow* window_create(char* title);

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

int targetFPS = 144;

const char* vert_shader_path = "./resources/shaders/vert_shader.vert";
const char* framg_shader_path = "./resources/shaders/fragm_shader.frag";
const char* tga_image_path = "./resources/dwsample-tga-640.tga";
const char* png_image_path = "./resources/test.png";

FILE* log_file_ptr;
const char* log_file_name = "log.txt";
const char* log_file_mode = "w";

void update_camera_matrix(ecs_iter_t* it);
void player_move_camera(ecs_iter_t* it);

GLFWwindow* window;

int main()
{
	ecs_world_t* world = ecs_init();

	ECS_COMPONENT(world, CameraSetting);
	ECS_COMPONENT(world, CameraViewProj);
	ECS_COMPONENT(world, LocalTransfrom);
	ECS_COMPONENT(world, LocalToWorld);

	ECS_TAG(world, Camera);

	ECS_SYSTEM(world, update_camera_matrix, EcsOnUpdate, CameraViewProj, [in] CameraSetting, [in] LocalTransfrom);
	ECS_SYSTEM(world, player_move_camera, EcsOnUpdate, LocalTransfrom, CameraSetting);

	ecs_entity_t camera_entity = init_camera(world);

	logger_init();
	random_init();

	log_info("logger inited");

	char app_name[] = "Snake GLFW ( FPS: %i )";

	GLFW_init();
	window = window_create(app_name);

	log_info("init_OpenGL");
	init_OpenGL();

	log_info("load_resources");

	stbi_set_flip_vertically_on_load(true);

	// load images
	log_info("load image file");

	int width, height, nrChannels;


	unsigned char* image_data = stbi_load(png_image_path, &width, &height, &nrChannels, STBI_rgb_alpha);

	int defTextWidth = 10;
	int defTextHeigh = 10;

	log_info("create_color_texture");

	Color defaultColor = { 1,1,1,1 };
	Color* defaultTextureData = create_color_texture(defTextWidth, defTextHeigh, defaultColor);

	log_info("create textures");

	// create textures
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

	GLuint vertex_shader, fragment_shader, shaderProgram;

	//create shader prog
	log_info("read vertex_shader");
	vertex_shader = shader_load_from_file(vert_shader_path, GL_VERTEX_SHADER);
	log_info("read fragment_shader");
	fragment_shader = shader_load_from_file(framg_shader_path, GL_FRAGMENT_SHADER);

	log_info("glCreateProgram");
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertex_shader);
	glAttachShader(shaderProgram, fragment_shader);
	glLinkProgram(shaderProgram);

	GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

	//create meshes
	MeshData quad_mesh = { 0 };

	quad_mesh.indices = indices;
	quad_mesh.indicesLen = 6;
	quad_mesh.avoidFreeMemory = true;
	quad_mesh.vertices = vertices;
	quad_mesh.verticesLen = 4;

	EBOBuffer quad_buffer = create_element_array_buffer(quad_mesh);

	MeshData circle_mesh = { 0 };
	int seg = 20;
	circle_mesh = create_circle_mesh(seg);
	EBOBuffer circle_buffer = create_element_array_buffer(circle_mesh);

	//create render data
	RenderData quad_render = { 0 };
	quad_render.renderBuffer = quad_buffer;
	quad_render.texture = png_g_texture;
	RenderData circle_render = { 0 };
	circle_render.renderBuffer = circle_buffer;
	circle_render.texture = g_texture;

	LocalTransfrom img_transform = transform_default();
	img_transform.position.x = 0.2f;

	float angle = 0;//angle_to_radians(45);

	img_transform.rotation = angle;
	LocalTransfrom transformCircle = transform_default();

	char window_title[100] = "";

	sprintf(window_title, app_name, 0);

	while (!glfwWindowShouldClose(window))
	{
		glfwGetFramebufferSize(window, &width, &height);

		sprintf(window_title, app_name, app_fps());
		glfwSetWindowTitle(window, window_title);

		app_update_time();
		AppTime time = app_time_get();

		app_update_fps(time.delta_time);

		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ecs_progress(world, time.delta_time);

		const CameraSetting* camSetting = ecs_get(world, camera_entity, CameraSetting);
		CameraViewProj* cam_view_proj = ecs_get_mut(world, camera_entity, CameraViewProj);

		//start shader prog
		glUseProgram(shaderProgram);

		//update proj and view
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (const GLfloat*)cam_view_proj->proj);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (const GLfloat*)cam_view_proj->view);

		draw_renderer(&img_transform, &quad_render, &cam_view_proj->view, &cam_view_proj->proj, modelLoc);
		//draw_renderer(&transformCircle, &circle_render, &cam_view_proj->view, &cam_view_proj->proj, modelLoc);

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

	ecs_fini(world);

	//system("pause");
	exit(EXIT_SUCCESS);
}


Vec3 camera_front = { 0, 0, -1.0f };
Vec3 camera_up =	{ 0, 1.0f, 0 };
Vec3 camera_right = { 1.0f, 0.0f, 0 };


void update_camera_matrix(ecs_iter_t* it) {

	float width, height;

	glfwGetFramebufferSize(window, &width, &height);

	// Get fields from system query
	CameraViewProj* view_proj = ecs_field(it, CameraViewProj, 1);
	CameraSetting* sett = ecs_field(it, CameraSetting, 2);
	LocalTransfrom* trans = ecs_field(it, LocalTransfrom, 3);

	// Iterate matched entities
	for (int i = 0; i < it->count; i++)
	{
		CameraViewProj* data = &view_proj[i];

		if (sett[i].persMode == 0)
		{
			float otrhoFacotr = sett[i].orthoSize;

			//init projection
			float left = -1.0f * otrhoFacotr;
			float right = 1.0f * otrhoFacotr;
			float bottom = -1.0f * otrhoFacotr;
			float top = 1.0f * otrhoFacotr;

			mat4x4_ortho(data->proj, left, right, bottom, top, sett[i].nearPlane, sett[i].farPlane);
		}
		else
		{
			mat4x4_identity(data->proj);
			mat4x4_perspective(data->proj, sett[i].fov, width / height, sett[i].nearPlane, sett[i].farPlane);
		}


		Vec3 center = { 0 };

		vec3_add(&center, &trans[i].position, &camera_front);

		mat4x4_translate(data->view, trans[i].position.x, trans[i].position.y, trans[i].position.z);
		mat4x4_look_at(data->view, (float*)&trans[i].position, (float*)&center, (float*)&camera_up);	
		mat4x4_rotate_Z(data->view, data->view, trans[i].rotation);
	}
}

void player_move_camera(ecs_iter_t* it) {

	float delta_time = it->delta_time;
	float speed = 2.5f * delta_time;

	LocalTransfrom* trans = ecs_field(it, LocalTransfrom, 1);

	for (int i = 0; i < it->count; i++)
	{	
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			Vec3 vec = camera_up;
			vec3_mul_value(&vec, &vec, speed);
			vec3_add(&trans[i].position, &trans[i].position, &vec);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			Vec3 vec = camera_up;
			vec3_mul_value(&vec, &vec, speed);
			vec3_sub(&trans[i].position, &trans[i].position, &vec);
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			Vec3 vec = camera_right;
			vec3_mul_value(&vec, &vec, speed);
			vec3_add(&trans[i].position, &trans[i].position, &vec);
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			Vec3 vec = camera_right;
			vec3_mul_value(&vec, &vec, speed);
			vec3_sub(&trans[i].position, &trans[i].position, &vec);
		}

		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		{
			trans->rotation += speed;
		}
	}
}

void logger_init()
{
	log_file_ptr = fopen(log_file_name, log_file_mode);

	if (log_file_ptr == NULL)
	{

		return;
	}
	log_info("log linked to file: %s", log_file_name);
	log_add_fp(log_file_ptr, 0);
}

void logger_free()
{
	if (log_file_ptr == NULL)
		return;

	fclose(log_file_ptr);
}

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void draw_renderer(const LocalTransfrom* trans, const RenderData* renderData, const mat4x4* view, const mat4x4* proj, const GLuint modevMatPath)
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
	glfwSwapInterval(1);
	log_info("Window Created");
	return window;
}

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