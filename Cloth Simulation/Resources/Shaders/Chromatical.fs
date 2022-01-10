#version 450 core

in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D tex;
uniform float currentTime;

void main(void)
{
	// Distance from center of the image to adjust the blur
	float d = length(fragTexCoord - vec2(0.5f, 0.5f));
	
	// blur amount
	float blur = 0.0f;	
	blur = (1.0 + sin(currentTime * 6.0)) * 0.5f;
	blur *= 1.0 + sin(currentTime * 16.0) * 0.5f;
	blur = pow(blur, 3.0f);
	blur *= 0.05f;
	
	// reduce blur towards center
	blur *= d;
	
	// final color
    vec3 col;
    col.r = texture(tex, vec2(fragTexCoord.x + blur, fragTexCoord.y)).r;
    col.g = texture(tex, fragTexCoord).g;
    col.b = texture(tex, vec2(fragTexCoord.x - blur, fragTexCoord.y)).b;
	
	// scanline
	float scanline = sin(fragTexCoord.y * 800.0f) * 0.04f;
	col -= scanline;
	
	// vignette
	col *= 1.0f - d * 0.5f;
	
	// Final color
    color = vec4(col, 1.0f);
}