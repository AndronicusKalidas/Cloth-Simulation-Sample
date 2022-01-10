#version 450 core

layout (location = 0) in vec3 position;

uniform mat4 PVM;

void main()
{
	gl_Position = vec4(position, 1.0);
}