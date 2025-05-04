#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoords;

out vec3 ourColor;
out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    ourColor = aColor;
    gl_Position = vec4(aPos, 0.0, 1.0);
}