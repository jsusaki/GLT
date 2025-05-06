#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 uv;

out vec3 vs_position;
out vec3 vs_normal;
out vec4 vs_color;
out vec2 vs_uv;

uniform mat4 model;
uniform mat4 proj_view;

void main()
{
	vs_position = vec3(model * vec4(position, 1.0));
	vs_normal   = mat3(transpose(inverse(model))) * normal;
	gl_Position = proj_view * vec4(vs_position, 1.0f);

	vs_color    = color;	
}