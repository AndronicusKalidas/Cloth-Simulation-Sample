#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec4 vertPosition[];

out vec2 texCoord;

uniform mat4 PVM;

uniform float Size;

void main()
{
	float HalfSize = Size * 0.5f;

	vec2 p1 = vertPosition[0].xy + vec2(-HalfSize, -HalfSize);
	gl_Position = PVM * vec4(p1, 0.0f, 1.0f);
	texCoord = vec2(0.0f, 0.0f); 
	EmitVertex();
	
	vec2 p2 = vertPosition[0].xy + vec2(-HalfSize, HalfSize);
	gl_Position = PVM * vec4(p2, 0.0f, 1.0f);
	texCoord = vec2(0.0f, 1.0f); 
	EmitVertex();
	
	vec2 p3 = vertPosition[0].xy + vec2(HalfSize, -HalfSize);
	gl_Position = PVM * vec4(p3, 0.0f, 1.0f);
	texCoord = vec2(1.0f, 0.0f); 
	EmitVertex();
	
	vec2 p4 = vertPosition[0].xy + vec2(HalfSize, HalfSize);
	gl_Position = PVM * vec4(p4, 0.0f, 1.0f);
	texCoord = vec2(1.0f, 1.0f); 
	EmitVertex();

	EndPrimitive(); 
}