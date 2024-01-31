#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec4 vertexColor; // specify a color output to the fragment shader

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);

    vertexColor = vec4(aColor, 1.0); // set the output variable to a dark-red color
}