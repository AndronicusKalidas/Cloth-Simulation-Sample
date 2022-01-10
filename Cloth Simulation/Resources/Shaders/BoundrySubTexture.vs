#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 fragTexCoord;
out vec2 fragPosition;

uniform mat4 PVM;

void main()
{
	vec4 tempPos = PVM * vec4(position, 1.0);
	gl_Position = tempPos;
	fragPosition = tempPos.xy;
	fragTexCoord = texCoord;
}