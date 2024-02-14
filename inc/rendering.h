#ifndef GAME_RENDER_H
#define GAME_RENDER_H

#include <glad/glad.h>
#include "LocalTransform.h"
#include "flecs.h"
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

typedef struct
{
	float r;
	float g;
	float b;
	float a;

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
	int avoidFreeMemory;

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

typedef struct RenderImage
{
	GLuint texture;
	ecs_entity_t shader;
	int renderOrder;

} RenderImage;


typedef struct ShaderProg
{
	GLuint shaderID;

} ShaderProg;

void rendering_init();

RenderData create_renderer(MeshData mesh, GLuint texture);
void mesh_free(MeshData mesh);
void printf_color(Color* color);
Color* create_color_texture(int width, int height, Color color);

MeshData create_quad_mesh();
MeshData create_circle_mesh(int segments);
EBOBuffer create_element_array_buffer(MeshData mesh);

//shader
ShaderProg shader_create(const char* vert_path, const char* fragm_path);
GLuint shader_load_from_text(char* shader_text, GLenum shader_type);
GLuint shader_load_from_file(char* shader_path, GLenum shader_type);

#endif