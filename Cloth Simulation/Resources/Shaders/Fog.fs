#version 450 core

in vec2 fragTexCoord;
in vec4 worldLocation;

out vec4 color;

uniform sampler2D tex;
uniform vec3 cameraPos;

uniform vec4 fogColour = vec4(1.0f, 0.5f, 0.5f, 1.0f);
uniform float fogStart = 40.0f;
uniform float fogLength = 10.0f;

void main(void)
{
	float lerp = (distance(worldLocation.xyz, cameraPos) - fogStart) / fogLength;
	lerp = clamp(lerp, 0.0f, 1.0f);
	color = mix(texture(tex, fragTexCoord), fogColour, lerp);
}