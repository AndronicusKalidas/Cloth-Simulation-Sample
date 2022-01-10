#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 fragTexCoord;
out vec4 worldLocation;

uniform mat4 PVM;
uniform mat4 model;

void main()
{
	worldLocation = model * vec4(position, 1.0f);
	gl_Position = PVM * vec4(position, 1.0f);
	fragTexCoord = texCoord;
}