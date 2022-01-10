#version 450 core

in vec2 fragTexCoord;
in vec2 fragPosition;

out vec4 color;

uniform vec4 boundry;
uniform sampler2D tex;
uniform ivec4 rect; // x, y, width, height

void main(void)
{
	if (fragPosition.x > boundry.x &&
		fragPosition.y < boundry.y &&
		fragPosition.x < boundry.z &&
		fragPosition.y > boundry.w)
	{
		vec2 texSize = textureSize(tex, 0);
		color = texture(tex, (rect.xy / texSize) + fragTexCoord * (rect.zw / texSize));
	}
	else
	{
		color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	
	
}