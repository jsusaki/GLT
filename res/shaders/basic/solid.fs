#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Color;
in vec2 TexCoords;

void main()
{
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}