#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 8) out;

out vec2 fragCoord;

in vec4 position[];

uniform mat4 PVM;
uniform float HeightThreshold;

void main()
{
	if (position[0].y < HeightThreshold)
	{
		vec4 center = PVM * position[0];

		float grassSize = 0.1f;

		gl_Position = center + PVM * vec4(-grassSize, grassSize, -grassSize, 1.0f);
		fragCoord = vec2(0.0f, 0.0f);
		EmitVertex();

		gl_Position = center + PVM * vec4(-grassSize, 0.0f, -grassSize, 1.0f);
		fragCoord = vec2(0.0f, 1.0f);
		EmitVertex();
		
		gl_Position = center + PVM * vec4(grassSize, 0.0f, grassSize, 1.0f);
		fragCoord = vec2(1.0f, 1.0f);
		EmitVertex();

		gl_Position = center + PVM * vec4(grassSize, grassSize, grassSize, 1.0f);
		fragCoord = vec2(1.0f, 0.0f);
		EmitVertex();

		EndPrimitive();
	
		gl_Position = center + PVM * vec4(grassSize, grassSize, -grassSize, 1.0f);
		fragCoord = vec2(0.0f, 0.0f);
		EmitVertex();

		gl_Position = center + PVM * vec4(grassSize, 0.0f, -grassSize, 1.0f);
		fragCoord = vec2(0.0f, 1.0f);
		EmitVertex();
		
		gl_Position = center + PVM * vec4(-grassSize, 0.0f, grassSize, 1.0f);
		fragCoord = vec2(1.0f, 1.0f);
		EmitVertex();

		gl_Position = center + PVM * vec4(-grassSize, grassSize, grassSize, 1.0f);
		fragCoord = vec2(1.0f, 0.0f);
		EmitVertex();

		EndPrimitive();
	}
}