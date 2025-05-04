#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoords;

uniform sampler2D screen_texture;

void main()
{
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}