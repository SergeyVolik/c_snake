#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 vertexColor; // specify a color output to the fragment shader
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);

    vertexColor = vec4(aColor, 1.0); // set the output variable to a dark-red color
    TexCoord = aTexCoord;
}