#ifndef GAME_RENDER_H
#define GAME_RENDER_H

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "LocalTransform.h"
#include "flecs.h"
#include "collection.h"
#include "game_math.h"

typedef struct
{
	float r;
	float g;
	float b;
	float a;

} Color;

typedef struct
{
	float3 vec;
	Color color;
	float2 uv;

} Vertex;

typedef struct MeshData
{
	Vertex* vertices;
	unsigned int verticesLen;
	int* indices;
	unsigned int indicesLen;
	int avoidFreeMemory;

} MeshData;

typedef struct Material
{
	ecs_entity_t shaderE;

} Material;

typedef struct
{
	GLuint VBO;
	GLuint VAO;
	GLuint EBO;
	MeshData meshData;

} EBOBuffer;

typedef struct
{
	GLuint VBO;
	GLuint VAO;
	int len;
} LineRenderBuffers;

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
	Color color;

} RenderImage;

typedef struct ShaderProg
{
	GLuint shaderID;
	int shaderOrder;
	ecs_entity_t shaderEntity;

} ShaderProg;

typedef struct ShaderArray
{
	DynamicBuffer array;

} ShaderArray;

typedef struct ShaderRenderOrderData
{
	ecs_entity_t renderEntity;
	int renderOrder;

} ShaderRenderOrderData;

typedef struct ShaderRenderArray
{
	DynamicBuffer array;

} ShaderRenderArray;

static int shader_array_compare(const void* a, const void* b)
{
	ShaderProg* l = ((ShaderProg*)a);
	ShaderProg* r = ((ShaderProg*)b);

	return (l->shaderOrder - r->shaderOrder);
}

static int comapre_int(const void* a, const void* b)
{
	int l = *((int*)a);
	int r = *((int*)b);

	return (l - r);
}

inline static void shader_array_sort(ShaderArray* array)
{	
	qsort(array->array.data->rawData, 2, sizeof(ShaderProg), shader_array_compare);
}

static int render_array_compare(const void* a, const void* b)
{
	int l = ((ShaderRenderOrderData*)a)->renderOrder;
	int r = ((ShaderRenderOrderData*)b)->renderOrder;

	return (l - r);
}

inline static void render_array_sort(ShaderRenderArray* array)
{
	//_qsort(array->data->rawData, array->data->element_size, 0, array->data->count, render_array_compare);
	//qsort(array->data->rawData, array->data->count, array->data->element_size, render_array_compare);
}

void rendering_init();

RenderData create_renderer(MeshData mesh, GLuint texture);
void mesh_free(MeshData mesh);
void printf_color(Color* color);
Color* create_color_texture(int width, int height, Color color);

MeshData create_quad_mesh();
MeshData create_circle_mesh(int segments);
EBOBuffer create_element_array_buffer(MeshData mesh);
LineRenderBuffers create_line_buffer(float3* points, int len);

//shader
GLuint load_shader(const char* vert_path, const char* fragm_path);
ecs_entity_t shader_create(const char* vert_path, const char* fragm_path, ShaderProg* shader, ecs_world_t* world, char* shaderName);
GLuint shader_load_from_text(char* shader_text, GLenum shader_type);
GLuint shader_load_from_file(char* shader_path, GLenum shader_type);

inline static Color color_new(float r, float g, float b, float a)
{
	Color color = { r, g,b,a };
	return color;
}

inline static Color color_red()
{	
	return color_new(1.0f, 0, 0 ,1.0f);
}

inline static Color color_green()
{
	return color_new(0, 1.0f, 0, 1.0f);
}

inline static Color color_blue()
{
	return color_new(0, 0, 1.0f, 1.0f);
}

inline static Color color_black()
{
	return color_new(0, 0, 0, 1.0f);
}

inline static Color color_white()
{
	return color_new(1, 1, 1, 1);
}

inline static Color color_rnd()
{
	return color_new(random_float(0, 1), random_float(0, 1), random_float(0, 1), 1);
}

#endif