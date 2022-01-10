#version 450 core

layout (location = 0) in vec3 vertex;

out vec4 position;

void main()
{
	gl_Position = vec4(vertex, 1.0);
	position = gl_Position;
}