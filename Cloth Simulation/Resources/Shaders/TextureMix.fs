#version 450 core

in vec2 fragTexCoord;

out vec4 color;

uniform float currentTime;
uniform sampler2D tex;
uniform sampler2D tex1;

void main(void)
{
	color = mix(texture(tex, fragTexCoord), texture(tex1, fragTexCoord), 0.5f * sin(currentTime) + 0.5f);
}