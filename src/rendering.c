#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "rendering.h"
#include "log.h"
#include "game_math.h"
#include "file_helper.h"
#include "flecs.h"

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

MeshData create_quad_mesh()
{
	//create meshes
	MeshData quad_mesh = { 0 };

	quad_mesh.indices = indices;
	quad_mesh.indicesLen = 6;
	quad_mesh.avoidFreeMemory = true;
	quad_mesh.vertices = vertices;
	quad_mesh.verticesLen = 4;

	return quad_mesh;
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

RenderData create_renderer(MeshData mesh, GLuint texture)
{
	EBOBuffer buff = create_element_array_buffer(mesh);

	RenderData data = {
		.renderBuffer = buff,
		.texture = texture
	};

	return data;
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

void rendering_init()
{
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

ecs_entity_t shader_create(const char* vert_path, const char* fragm_path, ShaderProg* shader, ecs_world_t* world, char* shaderName)
{
	ECS_COMPONENT(world, ShaderProg);

	GLuint vertex_shader, fragment_shader;

	//create shader prog
	//log_info("read vertex_shader");
	vertex_shader = shader_load_from_file(vert_path, GL_VERTEX_SHADER);
	//log_info("read fragment_shader");
	fragment_shader = shader_load_from_file(fragm_path, GL_FRAGMENT_SHADER);

	shader->shaderID = glCreateProgram();
	glAttachShader(shader->shaderID, vertex_shader);
	glAttachShader(shader->shaderID, fragment_shader);
	glLinkProgram(shader->shaderID);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	ecs_entity_t shader_e = ecs_new(world, ShaderProg);
	shader->shaderEntity = shader_e;

	ecs_set_name(world, shader_e, shaderName);
	ecs_set_id(world, shader_e, ecs_id(ShaderProg), sizeof(ShaderProg), shader);

	return shader_e;
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
	char* shader_text = file_read_all_text(shader_path);
	GLuint shader = shader_load_from_text(shader_text, shader_type);
	//log_info("free file text");
	free(shader_text);

	return shader;
}