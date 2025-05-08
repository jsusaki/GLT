#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec2 aTexCoords;

out vec3 Normal;
out vec4 Color;
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(aPos, 1.0);

    //gl_Position = vec4(aPos, 1.0);
    //gl_Position = projection * view * model * vec4(aPos, 1.0);

    Normal = aNormal;
    Color = aColor;
    TexCoords = aTexCoords;
}