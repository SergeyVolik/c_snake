#include "rendering.h"
#include <stdio.h>
#include "game_math.h"
#include <stdlib.h>

void printf_color(Color* color)
{
	printf("Color(%f, %f, %f, %f)\n", color->r, color->g, color->b, color->a);
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
	MeshData mesh = { 0 };

	float l = 0.5f;
	float a = PI * 2 / segments;

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

	int len = arrayLen - 1;

	for (int i = 0; i < len; i += 3)
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