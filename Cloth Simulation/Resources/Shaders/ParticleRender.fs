#version 450 core

in vec2 texCoord;

out vec4 color;

uniform sampler2D ParticleTexture;

void main(void)
{
	color = texture(ParticleTexture, texCoord);
}