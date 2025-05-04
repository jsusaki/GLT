#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screen_texture;

void main()
{
    vec3 texture_color = texture(screen_texture, TexCoords).rgb;
    FragColor = vec4(texture_color , 1.0);
}
