#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 colour;

out VS_GS_VERTEX{
	out vec4 position;
	out vec4 colour;
} vs_out;

uniform mat4 PVM;

void main()
{
	gl_Position = PVM * vec4(position, 1.0);

	vs_out.position = gl_Position;
	vs_out.colour = colour;
}