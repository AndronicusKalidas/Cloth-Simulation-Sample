#version 420 core
const int MAX_JOINTS = 100; // max joints allowed in a skeleton
const int MAX_WEIGHTS = 4; // max number of joints that can affect a vertex

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in ivec4 boneIds;
layout (location = 4) in vec4 weights;

uniform mat4 PVM;
uniform mat4 jointTransforms[MAX_JOINTS];

void main(void)
{
	vec4 totalLocalPosition = vec4(0.0);

	for(int i = 0; i < MAX_WEIGHTS; i++)
	{
		vec4 posePosition = jointTransforms[boneIds[i]] * vec4(position, 1.0);
		totalLocalPosition += posePosition * weights[i];
	}

	gl_Position = PVM * totalLocalPosition;
}