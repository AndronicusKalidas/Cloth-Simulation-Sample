#version 450 core

layout (location = 0) in vec3 position;

out vec4 vertPosition;

uniform mat4 PVM;

void main()
{
	vertPosition = vec4(position, 1.0f);
	gl_Position = PVM * vertPosition;
}