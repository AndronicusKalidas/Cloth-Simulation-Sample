#version 440 core

#define WORK_GROUP_SIZE 128
#define MAX_UINT 4294967295.0f;

layout(local_size_x = WORK_GROUP_SIZE, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) buffer positionBuffer{ vec3 position []; };
layout (std430, binding = 1) buffer velocityBuffer{ vec3 velocity []; };
layout (std430, binding = 2) buffer initVelocityBuffer{ vec3 initVelocity []; };

uniform float DeltaTime;
uniform vec3 Acceleration;
uniform uint RandomSeed;

uniform vec4 SpawnArea; // { x, z, width, length }
uniform float SpawnHeight; // Height of the spawn area
uniform float DespawnHeight; // Height of the despawn area

uint wang_hash(uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

void main()
{
    uint i = gl_GlobalInvocationID.x;

    velocity[i] += Acceleration * DeltaTime;
    position[i] += velocity[i] * DeltaTime;

    if (position[i].y <= DespawnHeight)
    {
        // Generate a random location
        float randX = wang_hash(RandomSeed + gl_GlobalInvocationID.x * 999) / MAX_UINT;
        float randZ = wang_hash(RandomSeed + gl_GlobalInvocationID.x * 999999) / MAX_UINT;

        float locX = SpawnArea.x + randX * SpawnArea.z;
        float locZ = SpawnArea.y + randZ * SpawnArea.w;
        
        position[i] = vec3(locX, SpawnHeight, locZ);
        velocity[i] = initVelocity[i];
    }
}