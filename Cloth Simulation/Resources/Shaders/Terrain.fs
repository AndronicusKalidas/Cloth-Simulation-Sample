#version 450 core

in vec2 fragCoord;

out vec4 color;

uniform sampler2D tex;

void main(void)
{
	vec4 tempCol = texture(tex, fragCoord);

	if (tempCol.w == 0.0f)
		discard;

	color = texture(tex, fragCoord);
}