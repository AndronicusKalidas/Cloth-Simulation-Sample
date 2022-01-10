#version 440 core

#define WORK_GROUP_SIZE 128

layout(local_size_x = WORK_GROUP_SIZE, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) buffer positionBuffer{ vec2 position []; };

uniform float DeltaTime;

void main()
{
    uint i = gl_GlobalInvocationID.x;

    position[i] += vec2(0.0f, 1.0f) * DeltaTime;
}