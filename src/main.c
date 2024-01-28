
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

#include <stdio.h>

typedef struct Vertex
{
    float x, y;
    float r, g, b;
} Vertex;

Vertex vertices[6] =
{
    //triang1
    { -1, -1, 0.f, 0.f, 1.f },
    { 1, -1.f, 0.f, 0.f, 1.f },
    { 1, 1, 0.f, 0.f, 1.f },
    //triang2
    { -1, -1, 0.f, 1.f, 0.f },
    { -1, 1.f, 0.f, 1.f, 0.f },
    { 1, 1, 0.f, 1.f, 0.f },
};

//Vertex vertices[3] =
//{
//    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
//    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
//    {   0.f,  0.6f, 0.f, 0.f, 1.f }
//};

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

char* read_file(char* path);

void logger_create();
void logger_free();

int main(void)
{
    logger_create();
    log_info("logger inited");

    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;

    char* vert_shader_path = "./shaders/vert_shader.vert";
    char* framg_shader = "./shaders/fragm_shader.frag";

    log_info("read file: %s", vert_shader_path);
    char* vertex_shader_text = read_file(vert_shader_path);
    log_info(vertex_shader_text);

    log_info("read file: %s", framg_shader);
    char* fragment_shader_text = read_file(framg_shader);
    log_info(fragment_shader_text);

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        log_error("glfwInit failed!");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    log_info("glfwCreateWindow");
    window = glfwCreateWindow(640, 480, "Snake GLFW", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    //glfwLoadTexture2D();
    // NOTE: OpenGL error checks have been omitted for brevity

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
        sizeof(vertices[0]), (void*)0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
        sizeof(vertices[0]), (void*)(sizeof(float) * 2));

    while (!glfwWindowShouldClose(window))
    {
        float vratio;
        float hratio;

        int width, height;
        mat4x4 m, p, mvp;

        glfwGetFramebufferSize(window, &width, &height);

        vratio = width / (float)height;
        hratio = height / (float)width;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        mat4x4_identity(m);
        //mat4x4_rotate_Z(m, m, (float)glfwGetTime());
        mat4x4_ortho(p, -1, 1, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    log_info("destroy glfw window");
    glfwDestroyWindow(window);

    glfwTerminate();

    log_info("free vertex file buffers");
    free(vertex_shader_text);
    free(fragment_shader_text);

    logger_free();

    system("pause");
    exit(EXIT_SUCCESS);
}

char* read_file(char* path)
{
    FILE* fp;
    long lSize;
    char* buffer;

    fp = fopen(path, "rb");
    if (!fp)
    {
        log_error("file cant be open. file:  %s", path);
        perror(path);
        exit(1);
    }

    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);

    /* allocate memory for entire content */
    buffer = calloc(1, lSize + 1);

    if (!buffer)
    {
        log_error("memory alloc fails. file: %s", path);
        fclose(fp);
        fputs("memory alloc fails", stderr);
        exit(1);
    }
    /* copy the file into the buffer */
    if (1 != fread(buffer, lSize, 1, fp))
    {
        fclose(fp);
        free(buffer);
        log_error("entire read fails file: %s", path);
        fputs("entire read fails", stderr);
        exit(1);
    }

    /* do your work here, buffer is a string contains the whole text */

    fclose(fp);

    return buffer;
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