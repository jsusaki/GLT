#version 330 core

out vec4 FragColor;

uniform vec4 color;

void main() 
{
    vec2 d = gl_PointCoord - vec2(0.5);
    if (length(d) > 0.5)
        discard;
    FragColor = color;
}
