#ifndef GAME_RENDER_H
#define GAME_RENDER_H

#include <glad/glad.h>
#include "LocalTransform.h"

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

typedef struct
{
	GLuint shaderProgram;

} Shader;

void mesh_free(MeshData mesh);
void printf_color(Color* color);
Color* create_color_texture(int width, int height, Color color);
MeshData create_circle_mesh(int segments);
EBOBuffer create_element_array_buffer(MeshData mesh);

#endif