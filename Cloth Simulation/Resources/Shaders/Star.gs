#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 100) out;

out vec4 outColor;

in VS_GS_VERTEX{
	in vec4 position;
	in vec4 colour;
} gs_in[];

#define pi 3.1415926535897932384626433832795

uniform mat4 PVM;

uniform int Spikes;
uniform float InnerRadius;
uniform float OuterRadius;

vec4 LengthDir(float length, float angle)
{
	return vec4(cos(angle), 0.0f, sin(angle), 0.0f) * length;
}

void main()
{
	vec4 center = gs_in[0].position;
	float angleIncr = (2.0f * pi) / (Spikes * 2.0f);

	for (int i = 0; i < Spikes; ++i)
	{
		float angle = angleIncr * i * 2.0f;
		
		gl_Position = center + PVM * LengthDir(InnerRadius, angle);
		outColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		EmitVertex();

		angle += angleIncr;
		gl_Position = center + PVM * LengthDir(OuterRadius, angle);
		outColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		EmitVertex();
		
		gl_Position = center;
		outColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		EmitVertex();

		angle += angleIncr;
		gl_Position = center + PVM * LengthDir(InnerRadius, angle);
		outColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		EmitVertex();
		

		EndPrimitive();
	}
}