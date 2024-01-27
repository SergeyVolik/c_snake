// Snake.cpp : This file contains the 'main' function. Program execution begins and ends there.

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
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

#include <stdio.h>

static const struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

char* read_file(char* path);

void logger_create();
void logger_free();

int main(void)
{
    logger_create();

    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;

    log_info("destroy window");
    system("pause");
    //char* vert_shader_path = "shaders/vert_shader.vert";
    //char* framg_shader = "shaders/framg_shader.vert";

    //log_info(strcat("read file: ", vert_shader_path));
    //char* vertex_shader_text = read_file("shaders/vert_shader.vert");
    //log_info(strcat("read file: ", framg_shader));
    //char* fragment_shader_text = read_file("shaders/framg_shader.vert");

    //glfwSetErrorCallback(error_callback);

    //if (!glfwInit())
    //    exit(EXIT_FAILURE);

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    //window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    //if (!window)
    //{
    //    glfwTerminate();
    //    exit(EXIT_FAILURE);
    //}

    //glfwSetKeyCallback(window, key_callback);

    //glfwMakeContextCurrent(window);
    //gladLoadGL(glfwGetProcAddress);
    //glfwSwapInterval(1);

    //// NOTE: OpenGL error checks have been omitted for brevity

    //glGenBuffers(1, &vertex_buffer);
    //glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    //glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    //glCompileShader(vertex_shader);

    //fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    //glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    //glCompileShader(fragment_shader);

    //program = glCreateProgram();
    //glAttachShader(program, vertex_shader);
    //glAttachShader(program, fragment_shader);
    //glLinkProgram(program);

    //mvp_location = glGetUniformLocation(program, "MVP");
    //vpos_location = glGetAttribLocation(program, "vPos");
    //vcol_location = glGetAttribLocation(program, "vCol");

    //glEnableVertexAttribArray(vpos_location);
    //glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
    //    sizeof(vertices[0]), (void*)0);
    //glEnableVertexAttribArray(vcol_location);
    //glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
    //    sizeof(vertices[0]), (void*)(sizeof(float) * 2));

    //while (!glfwWindowShouldClose(window))
    //{
    //    float ratio;
    //    int width, height;
    //    mat4x4 m, p, mvp;

    //    glfwGetFramebufferSize(window, &width, &height);
    //    ratio = width / (float)height;

    //    glViewport(0, 0, width, height);
    //    glClear(GL_COLOR_BUFFER_BIT);

    //    mat4x4_identity(m);
    //    mat4x4_rotate_Z(m, m, (float)glfwGetTime());
    //    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    //    mat4x4_mul(mvp, p, m);

    //    glUseProgram(program);
    //    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
    //    glDrawArrays(GL_TRIANGLES, 0, 3);

    //    glfwSwapBuffers(window);
    //    glfwPollEvents();
    //}

    //log_info("destroy window");
    //glfwDestroyWindow(window);

    //glfwTerminate();

    //log_info("free vertex file buffers");
    //free(vertex_shader_text);
    //free(fragment_shader_text);

    logger_free();
    exit(EXIT_SUCCESS);
}

char* read_file(char* path)
{
    FILE* fp;
    long lSize;
    char* buffer;


    fp = fopen(path, "rb");
    if (!fp) perror(path), exit(1);

    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);

    /* allocate memory for entire content */
    buffer = calloc(1, lSize + 1);
    if (!buffer)
    {
        fclose(fp);
        fputs("memory alloc fails", stderr);
        system("pause");
        exit(1);
    }
    /* copy the file into the buffer */
    if (1 != fread(buffer, lSize, 1, fp))
        fclose(fp), free(buffer), fputs("entire read fails", stderr), exit(1);

    /* do your work here, buffer is a string contains the whole text */

    fclose(fp);

    return buffer;
}


FILE* fptr;

const char* log_file_name = "log.txt";
const char* file_mode = "w";

void logger_create()
{
    FILE* fptr;
    fptr = fopen(log_file_name, file_mode);
    log_add_fp(fptr, 0);
}

void logger_free()
{
    fclose(fptr);
}