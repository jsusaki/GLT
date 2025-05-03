#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screen_texture;

void main()
{
    vec3 color = texture(screen_texture, TexCoords).rgb;
    FragColor = vec4(color, 1.0);
}
