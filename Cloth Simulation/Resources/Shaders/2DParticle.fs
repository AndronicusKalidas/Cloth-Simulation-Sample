#version 450 core

in vec2 texCoord;

out vec4 color;

//uniform sampler2D ParticleTexture;

void main(void)
{
	color = vec4(1.0f, 0.0f, 0.0f, 1.0f);//texture(ParticleTexture, texCoord);
}