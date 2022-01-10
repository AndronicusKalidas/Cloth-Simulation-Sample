#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 100) out;

out vec4 outColor;

in VS_GS_VERTEX{
	in vec4 position;
	in vec4 colour;
} gs_in[];

#define M_PI 3.1415926535897932384626433832795

uniform sampler2D heightMap;
uniform mat4 PVM;

uniform vec4 FacingVec;
uniform vec4 NormalVec;

uniform float GrassAngle;
uniform float GrassWidth;
uniform float GrassHeight;
uniform int Segments;

vec4 GetDir(vec2 uvPos)
{
   float height = texture(heightMap, uvPos).x;

   // Compute the differentials by stepping over 1 in both directions.
   float dx = texture(heightMap, uvPos + vec2(0.1f, 0.0f)).x - height;
   float dy = texture(heightMap, uvPos + vec2(0.0f, 0.1f)).x - height;

   // The "steepness" is the magnitude of the gradient vector
   // For a faster but not as accurate computation, you can just use abs(dx) + abs(dy)
   vec2 direct = normalize(vec2(dx, dy));

   return vec4(direct.x, 0.0f, direct.y, 1.0f);
}

void main()
{
	vec4 mvpPos = PVM * gs_in[0].position;

	vec2 uvPos = (gs_in[0].position.xz + vec2(5.0f, 5.0f)) * 0.1f;
	float gHeight = texture(heightMap, uvPos).x * GrassHeight;
	vec4 gFacing = GetDir(uvPos);

	vec4 rightVec = vec4(normalize(cross(NormalVec.xyz, gFacing.xyz)) * GrassWidth, 0.0f);

	vec3 currentLoc = vec3(0.0f, 0.0f, 0.0f);
	float currentAngle = M_PI * 0.5f;
	float heightIncr = gHeight / Segments;
	float angleIncr = GrassAngle / Segments;

	for (int i = 0; i < Segments; ++i)
	{
		float t = (i / float(Segments));

		gl_Position = mvpPos + PVM * (vec4(currentLoc, 1.0f) + rightVec *  (1.0f - t));
		outColor = vec4(gs_in[0].colour.xyz * t, 1.0f);
		EmitVertex();
		
		gl_Position = mvpPos + PVM * (vec4(currentLoc, 1.0f) - rightVec * (1.0f - t));
		outColor = vec4(gs_in[0].colour.xyz * t, 1.0f);
		EmitVertex();
		
		currentAngle += angleIncr;
		currentLoc += normalize(gFacing.xyz * cos(currentAngle) + NormalVec.xyz * sin(currentAngle)) * heightIncr;
	}
	
	//currentLoc += normalize(gFacing.xyz * cos(currentAngle) + NormalVec.xyz * sin(currentAngle)) * heightIncr;
	gl_Position = mvpPos + PVM * (vec4(currentLoc, 1.0f));
	outColor = vec4(gs_in[0].colour.xyz, 1.0f);
	EmitVertex();
	
	EndPrimitive();
}