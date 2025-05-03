#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D screen_texture;

void main()
{
    FragColor = texture(screen_texture, TexCoord);
}
