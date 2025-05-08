#version 330 core

layout(location = 0) in vec2 aPos;

uniform mat4 projection;
uniform float size;

void main() 
{
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    gl_PointSize  = size;
}