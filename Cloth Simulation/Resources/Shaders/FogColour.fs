#version 450 core

in vec2 fragTexCoord;
in vec4 worldLocation;

out vec4 color;

uniform vec4 objColour;
uniform vec3 camPos;

uniform vec4 fogColour = vec4(1.0f, 0.5f, 0.5f, 1.0f);
uniform float fogStart = 40.0f;
uniform float fogLength = 10.0f;

void main(void)
{
	float lerp = (distance(worldLocation.xyz, camPos) - fogStart) / fogLength;
	lerp = clamp(lerp, 0.0f, 1.0f);
	color = mix(objColour, fogColour, lerp);
}