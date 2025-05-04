#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoords;

uniform sampler2D screen_texture;

void main()
{
    vec3 texture_color = texture(screen_texture, TexCoords).rgb;

    // Grayscale effect: luminance formula
    vec3 luminance  = vec3(0.299, 0.587, 0.114);
    float grayscale = dot(texture_color, luminance);

    FragColor = vec4(vec3(grayscale), 1.0);
}