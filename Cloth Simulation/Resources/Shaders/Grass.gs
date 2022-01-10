#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 100) out;

out vec4 outColor;

in VS_GS_VERTEX{
	in vec4 position;
	in vec4 colour;
} gs_in[];

#define M_PI 3.1415926535897932384626433832795

uniform mat4 PVM;

uniform float GrassWidth;
uniform float GrassAngle;
uniform float GrassHeight;
uniform int Segments;

void main()
{
	vec3 currentColour = vec3(0.0f, 0.0f, 0.0f);
	float currentAngle = M_PI * 0.5f;
	float currentWidth = GrassWidth;

	float heightIncr = GrassHeight / Segments;
	float widthIncr = GrassWidth / Segments;
	vec3 colourIncr = vec3(gs_in[0].colour) / Segments;

	vec2 baseStem = vec2(0.0f, 0.0f);
	
	// Draw base two vertices of the grass blade
	gl_Position = gs_in[0].position + PVM * vec4(GrassWidth, 0.0f, 0.0f, 0.0f);
	outColor = vec4(currentColour, 1.0f);
	EmitVertex();

	gl_Position = gs_in[0].position + PVM * vec4(-GrassWidth, 0.0f, 0.0f, 0.0f);
	outColor = vec4(currentColour, 1.0f);
	EmitVertex();


	for (int i = 1; i < Segments; ++i)
	{
		currentColour += colourIncr;
		currentAngle += GrassAngle;

		vec2 increment = vec2(cos(currentAngle), sin(currentAngle)) * heightIncr;
		vec2 ptOffset = normalize(vec2(-increment.y, increment.x)) * currentWidth;
		baseStem += increment;

		gl_Position = gs_in[0].position + PVM * vec4(baseStem - ptOffset, 0.0f, 0.0f);
		outColor = vec4(currentColour, 1.0f);
		EmitVertex();
	
		gl_Position = gs_in[0].position + PVM * vec4(baseStem + ptOffset, 0.0f, 0.0f);
		outColor = vec4(currentColour, 1.0f);
		EmitVertex();

		currentWidth -= widthIncr;
	}

	currentColour += colourIncr;
	currentAngle += GrassAngle;
	baseStem += vec2(cos(currentAngle), sin(currentAngle)) * heightIncr;
	
	outColor = vec4(currentColour, 1.0f);
	gl_Position = gs_in[0].position + PVM * vec4(baseStem, 0.0f, 0.0f);
	EmitVertex();

	EndPrimitive();
}