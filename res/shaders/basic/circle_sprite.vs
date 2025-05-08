#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aVelocity;
layout(location = 2) in vec4 aColor;

out vec4 vColor;
out float vSpeed;

uniform mat4 projection;
uniform float size;
uniform float max_speed;

void main() 
{
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    gl_PointSize = size;
    vColor = aColor;
    vSpeed = clamp(length(aVelocity) / max_speed, 0.0, 1.0);
}