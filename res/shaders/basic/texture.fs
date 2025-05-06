#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Color;
in vec2 TexCoords;

uniform sampler2D screen_texture;

void main()
{
    FragColor = texture(screen_texture, TexCoords);
}