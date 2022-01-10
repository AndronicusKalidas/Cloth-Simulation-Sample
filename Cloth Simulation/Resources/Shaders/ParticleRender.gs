#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec4 vertPosition[];

out vec2 texCoord;

#define pi 3.1415926535897932384626433832795

uniform mat4 PVM;

uniform float Size;
uniform vec3 vQuad1;
uniform vec3 vQuad2;

void main()
{
	float HalfSize = Size * 0.5f;

	vec3 p1 = vertPosition[0].xyz +(-vQuad1 - vQuad2) * HalfSize;
	gl_Position = PVM * vec4(p1, 1.0f);
	texCoord = vec2(0.0f, 0.0f); 
	EmitVertex();

	vec3 p2 = vertPosition[0].xyz + (-vQuad1 + vQuad2) * HalfSize;
	gl_Position = PVM * vec4(p2, 1.0f);
	texCoord = vec2(0.0f, 1.0f); 
	EmitVertex();

	vec3 p3 = vertPosition[0].xyz + (vQuad1 - vQuad2) * HalfSize;
	gl_Position = PVM * vec4(p3, 1.0f);
	texCoord = vec2(1.0f, 0.0f); 
	EmitVertex();

	vec3 p4 = vertPosition[0].xyz + (vQuad1 + vQuad2) * HalfSize;
	gl_Position = PVM * vec4(p4, 1.0f);
	texCoord = vec2(1.0f, 1.0f); 
	EmitVertex();

	EndPrimitive(); 
}