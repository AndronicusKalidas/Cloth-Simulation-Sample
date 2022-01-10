#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 fragNormal;
out vec2 fragTexCoord;
out vec3 fragPos;
out vec4 fragPosLightSpace;

uniform mat4 Model;
uniform mat4 ModelInverse;

uniform mat4 PVM;
uniform mat4 LightPVM;

void main()
{
	vec4 pos = vec4(position, 1.0f);
	gl_Position = PVM * pos;
	
	fragNormal = vec3(ModelInverse * vec4(normal, 1.0f));
	fragTexCoord = texCoord;
	fragPos = vec3(Model * pos);
	fragPosLightSpace = LightPVM * pos;
}