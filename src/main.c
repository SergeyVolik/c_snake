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

typedef struct SnakeHead1
{
	float speed;
	Vec2 direction;
} SnakeHead1;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void glfw_error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void GLFW_init();
void init_OpenGL();
GLFWwindow* window_create(char* title);
void render_object_system(ecs_iter_t* it);
void render_view_system(ecs_iter_t* it);
void setup_render_buffer_system(ecs_iter_t* it);
void update_camera_matrix(ecs_iter_t* it);
void player_move(ecs_iter_t* it);
void cleanup_render_data(ecs_iter_t* it);
void delete_entity_sys(ecs_iter_t* it);
void draw_obj(LocalTransfrom* trans, RenderData* renderData);

int targetFPS = 144;

const char* vert_shader_default_path = "./resources/shaders/vert_shader_default.vert";
const char* framg_shader_default_path = "./resources/shaders/fragm_shader_default.frag";
const char* vert_shader_2_path = "./resources/shaders/vert_shader2.vert";
const char* framg_shader_2_path = "./resources/shaders/fragm_shader2.frag";
const char* tga_image_path = "./resources/dwsample-tga-640.tga";
const char* png_image_path = "./resources/test.png";



GLFWwindow* window;
ShaderProg shader_program;
ShaderProg shaderProgram2;

GLuint modelLoc;
GLuint viewLoc;
GLuint projectionLoc;
ecs_entity_t camera_entity;

Vec3 camera_front = { 0, 0, -1.0f };
Vec3 camera_up = { 0, 1.0f, 0 };
Vec3 camera_right = { 1.0f, 0.0f, 0 };

ecs_entity_t snake_entity_prefab;
ecs_world_t* world_def;

ECS_COMPONENT_DECLARE(CameraSetting);
ECS_COMPONENT_DECLARE(CameraViewProj);
ECS_COMPONENT_DECLARE(LocalTransfrom);
ECS_COMPONENT_DECLARE(RenderData);
ECS_COMPONENT_DECLARE(SnakeHead1);
ECS_COMPONENT_DECLARE(RenderImage);

ECS_TAG_DECLARE(Camera);
ECS_TAG_DECLARE(DeleteTag);

ecs_entity_t prefab_instantiate(ecs_world_t* world, ecs_entity_t prefab);
ecs_world_t* world_default();

ecs_world_t* world_default()
{
	return world_def;
}

ecs_entity_t prefab_instantiate(ecs_world_t* world, ecs_entity_t prefab)
{
	ecs_entity_t instance = ecs_new_w_pair(world, EcsIsA, prefab);

	return instance;
}

int main(int argc, char* argv[])
{
	world_def = ecs_init_w_args(argc, argv);

	// Start REST API with default parameters
	ecs_singleton_set(world_def, EcsRest, { 0 });

	ECS_IMPORT(world_def, FlecsMonitor);

	ECS_COMPONENT_DEFINE(world_def, CameraSetting);
	ECS_COMPONENT_DEFINE(world_def, CameraViewProj);
	ECS_COMPONENT_DEFINE(world_def, LocalTransfrom);
	ECS_COMPONENT_DEFINE(world_def, RenderData);
	ECS_COMPONENT_DEFINE(world_def, SnakeHead1);
	ECS_COMPONENT_DEFINE(world_def, RenderImage);

	ECS_TAG_DEFINE(world_def, Camera);
	ECS_TAG_DEFINE(world_def, DeleteTag);

	ECS_SYSTEM(world_def, update_camera_matrix, EcsOnUpdate, CameraViewProj, [in] CameraSetting, [in] LocalTransfrom);
	ECS_SYSTEM(world_def, player_move, EcsOnUpdate, LocalTransfrom, CameraSetting);
	

	
	ecs_entity_t test_sys = ecs_system(world_def, {
		.entity = ecs_entity(world_def, { /* ecs_entity_desc_t */
		.name = "setup_render_buffer_system",
		.add = { ecs_dependson(EcsOnUpdate) }
		}),
		.query.filter.terms = {
			{ecs_id(RenderImage),.oper = EcsAnd },
			{ecs_id(RenderData),.oper = EcsNot   },
		},
		.callback = setup_render_buffer_system
		});

	ECS_SYSTEM(world_def, render_view_system, EcsOnUpdate, CameraSetting, CameraViewProj);
	ECS_SYSTEM(world_def, render_object_system, EcsOnUpdate, LocalTransfrom, RenderData);
	
	//cleanup
	ECS_SYSTEM(world_def, cleanup_render_data, EcsOnUpdate, RenderData, DeleteTag);
	ECS_SYSTEM(world_def, delete_entity_sys, EcsOnUpdate, DeleteTag);


	//manula sys creation
	//ecs_entity_t test_sys = ecs_system(world, {
	//	.query.filter.terms = {
	//		{ecs_id(Comp),.oper = EcsAnd },
	//		{ecs_id(Comp),.oper = EcsOr   },
	//		{ecs_id(Comp), .oper = EcsNot  },
	//		{ecs_id(Comp), .oper = EcsOptional   },
	//	},
	//	.callback = sys_name
	//});

	camera_entity = init_camera(world_def);

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


	shader_program = shader_create(vert_shader_default_path, framg_shader_default_path);
	shaderProgram2 = shader_create(vert_shader_2_path, framg_shader_2_path);

	modelLoc = glGetUniformLocation(shader_program.shaderID, "model");
	viewLoc = glGetUniformLocation(shader_program.shaderID, "view");
	projectionLoc = glGetUniformLocation(shader_program.shaderID, "projection");

	LocalTransfrom img_transform = transform_default();
	img_transform.position.x = 1.0f;
	RenderImage img2 = { png_g_texture };
	ecs_entity_t reward_entity_prefab = ecs_new_w_id(world_def, EcsPrefab);

	ecs_set_id(world_def, reward_entity_prefab, ecs_id(LocalTransfrom), sizeof(LocalTransfrom), &img_transform);
	ecs_set_id(world_def, reward_entity_prefab, ecs_id(RenderImage), sizeof(RenderImage), &img2);

	SnakeHead1 snakeHead = { .speed = 1.0f, .direction = { 0, 1 } };

	snake_entity_prefab = ecs_new_w_id(world_def, EcsPrefab);
	RenderImage img = { g_texture };

	LocalTransfrom transform_snake = transform_default();
	ecs_set_id(world_def, snake_entity_prefab, ecs_id(LocalTransfrom), sizeof(LocalTransfrom), &transform_snake);
	ecs_set_id(world_def, snake_entity_prefab, ecs_id(SnakeHead1), sizeof(SnakeHead1), &snakeHead);
	ecs_set_id(world_def, snake_entity_prefab, ecs_id(RenderImage), sizeof(RenderImage), &img);

	ecs_entity_t snake_entity_instance = prefab_instantiate(world_default(), snake_entity_prefab);

	char window_title[50] = "";

	sprintf(window_title, app_name, 0);

	while (!glfwWindowShouldClose(window))
	{
		glfwGetFramebufferSize(window, &width, &height);

		sprintf(window_title, app_name, app_fps());
		glfwSetWindowTitle(window, window_title);

		
		app_update_time();
		AppTime time = app_time_get();

		float delta_time = time.delta_time;

		app_update_fps(delta_time);

		ecs_progress(world_def, delta_time);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	log_info("destroy glfw window");

	glDeleteProgram(shader_program.shaderID);
	glDeleteTextures(1, &png_g_texture);

	glfwDestroyWindow(window);

	glfwTerminate();

	logger_free();

	free(defaultTextureData);
	stbi_image_free(image_data);

	ecs_fini(world_def);

	exit(EXIT_SUCCESS);
}

void render_view_system(ecs_iter_t* it) {

	ECS_COMPONENT(it->world, CameraViewProj);

	CameraViewProj* cam_view_proj = ecs_get_mut(it->world, camera_entity, CameraViewProj);

	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//start shader prog
	glUseProgram(shader_program.shaderID);

	//update proj and view
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (const GLfloat*)cam_view_proj->proj);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (const GLfloat*)cam_view_proj->view);
}

void setup_render_buffer_system(ecs_iter_t* it) {

	ECS_COMPONENT(it->world, RenderData);

	RenderImage* imges = ecs_field(it, RenderImage, 1);

	for (int i = 0; i < it->count; i++)
	{
		RenderImage* trans = &imges[i];

		RenderData quad_render = create_renderer(create_quad_mesh(), trans->texture);
		ecs_set_id(it->world, it->entities[i], ecs_id(RenderData), sizeof(RenderData), &quad_render);
	}
}

void delete_entity_sys(ecs_iter_t* it) {

	for (size_t i = 0; i < it->count; i++)
	{
		log_info("delete entity");
		ecs_delete(it->world, it->entities[i]);
	}
}

void cleanup_render_data(ecs_iter_t* it) {

	RenderData* imges = ecs_field(it, RenderData, 1);

	for (size_t i = 0; i < it->count; i++)
	{
		log_info("delete render data");
		glDeleteBuffers(1, &imges[i].renderBuffer.VBO);
		glDeleteVertexArrays(1, &imges[i].renderBuffer.VAO);
	}
}

void render_object_system(ecs_iter_t* it) {

	LocalTransfrom* transfroms = ecs_field(it, LocalTransfrom, 1);
	RenderData* renderers = ecs_field(it, RenderData, 2);
	AppTime time = app_time_get();
	//log_info("render obj %i time: %f", it->count, time.total_time);

	for (int i = 0; i < it->count; i++)
	{
		LocalTransfrom* trans = &transfroms[i];
		RenderData* renderData = &renderers[i];

		draw_obj(trans, renderData);
	}
}

void draw_obj(LocalTransfrom* trans, RenderData* renderData)
{
	mat4x4 modelMat;

	mat4x4_translate_vec3(modelMat, &trans->position);
	mat4x4_rotate_Z(modelMat, modelMat, trans->rotation);
	mat4x4_scale(modelMat, modelMat, trans->scale);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (const GLfloat*)modelMat);

	glBindTexture(GL_TEXTURE_2D, renderData->texture);

	glBindVertexArray(renderData->renderBuffer.VAO);

	if (renderData->renderBuffer.EBO != NULL)
	{
		//log_info("%i", renderData->renderBuffer.meshData.indicesLen);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderData->renderBuffer.EBO);
		glDrawElements(GL_TRIANGLES, renderData->renderBuffer.meshData.indicesLen, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, renderData->renderBuffer.VBO);
		glDrawArrays(GL_TRIANGLES, 0, renderData->renderBuffer.meshData.verticesLen);
	}
}

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

		if (sett[i].persMode == false)
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

void player_move(ecs_iter_t* it) {

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
	}
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

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		log_info("create");

		LocalTransfrom transform_snake = transform_default();

		transform_snake.position.x = random_float(-6, 6);
		transform_snake.position.y = random_float(-6, 6);

		ecs_entity_t snake_entity_instance = ecs_new_w_pair(world_def, EcsIsA, snake_entity_prefab);
		ecs_set_id(world_def, snake_entity_instance, ecs_id(LocalTransfrom), sizeof(LocalTransfrom), &transform_snake);
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		ecs_filter_t* f = ecs_filter(world_def, {
		  .terms = {
			{ ecs_id(LocalTransfrom) },
			{ ecs_id(SnakeHead1) },
		  }
			});

		ecs_iter_t it = ecs_filter_iter(world_def, f);

		// Outer loop: matching tables
		while (ecs_filter_next(&it)) {

			for (size_t i = 0; i < it.count; i++)
			{
				ecs_add(world_def, it.entities[i], DeleteTag);
			}
		}
	}	
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
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

float lastX = 400, lastY = 300;
int firstMouse = 1;
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

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

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
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