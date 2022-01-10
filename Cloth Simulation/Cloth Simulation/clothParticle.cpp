#include "clothParticle.h"
#include "cloth.h"
#include "mesh.h"
#include "program.h"
#include "texture.h"
#include "timer.h"
#include "helper.h"

// Generate a particle (texture)
CClothParticle::CClothParticle(CCloth* parent, glm::vec3 location, glm::ivec2 gridLoc)
{
	Location = location;

	OldLocation = Location;
	OriginalLocation = Location;
	GridLocation = gridLoc;

	ForceMultiplier = glm::vec3(1.0f, 1.0f, 1.0f);
	Parent = parent;
	Mass = 1.0f;
	Damping = 0.999f;

	FireTick = 0.0f;
	IsOnFire = false;

	FireTickToBurn += HELPER::Random();
	FireTickToSpread += HELPER::Random();
}


CClothParticle::~CClothParticle()
{
}

void CClothParticle::Update()
{
	ApplyPhysics();

	if (IsOnFire)
	{
		float newVal = FireTick + TIMER::GetDeltaSeconds();

		if (FireTick < FireTickToSpread && newVal >= FireTickToSpread)
		{
			for (Link* link : Links)
			{
				link->P1->SetAflame();
				link->P2->SetAflame();
			}
		}

		FireTick = std::min(newVal, FireTickToBurn);
	}
}

float CClothParticle::GetMass()
{
	return Mass;
}

void CClothParticle::Move(glm::vec3 delta)
{
	Location += delta * ForceMultiplier;
}

void CClothParticle::SetForceMultiplier(float x, float y, float z)
{
	ForceMultiplier = glm::vec3(x, y, z);
}

glm::vec3 CClothParticle::GetForceMultiplier()
{
	return ForceMultiplier;
}

glm::vec3 CClothParticle::GetOriginalLocation()
{
	return OriginalLocation;
}

glm::ivec2 CClothParticle::GetGridLocation()
{
	return GridLocation;
}

float CClothParticle::GetFireTick()
{
	return FireTick / FireTickToBurn;
}

void CClothParticle::SetAflame()
{
	IsOnFire = true;
}

void CClothParticle::SolveSphereCollision(glm::vec3 location, float radius)
{
	glm::vec3 delta = Location - location;

	if (glm::length(delta) < radius)
	{
		glm::vec3 result = glm::normalize(delta) * radius;
		Location = location + result;
	}
}

void CClothParticle::SolvePyramidCollision(glm::vec3 location, float scale)
{
	glm::vec3 p1 = Location - (location + glm::vec3(0.0f, scale, 0.0f));
	glm::vec3 p2 = Location - location;

	std::vector<std::pair<glm::vec3, glm::vec3>> normals;
	normals.push_back(std::make_pair(glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)), p1));
	normals.push_back(std::make_pair(glm::normalize(glm::vec3(-1.0f, 1.0f, 0.0f)), p1));
	normals.push_back(std::make_pair(glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f)), p1));
	normals.push_back(std::make_pair(glm::normalize(glm::vec3(0.0f, 1.0f, -1.0f)), p1));

	normals.push_back(std::make_pair(glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)), p2));

	float shortestDist = (float)INT_MAX;
	glm::vec3 shortestNormal;

	for (std::pair<glm::vec3, glm::vec3>& n : normals)
	{
		float dist = glm::dot(n.first, n.second);

		if (dist >= 0)
		{
			return;	// no collision
		}
		else if (std::abs(dist) < std::abs(shortestDist))
		{
			shortestDist = dist;
			shortestNormal = n.first;
		}
	}

	Location -= shortestNormal * shortestDist;
}

void CClothParticle::SolveCapsuleCollision(glm::vec3 location, float length, float radius)
{
	glm::vec2 pos1 = glm::vec2(location.x, location.z);
	glm::vec2 pos2 = glm::vec2(Location.x, Location.z);

	glm::vec2 delta = pos2 - pos1;
	float dist = glm::length(pos1 - pos2);

	float halfLength = length * 0.5f;

	if (dist <= radius)
	{
		float deltaHeight = std::abs(location.y - Location.y);

		if (deltaHeight <= halfLength)
		{
			glm::vec2 result = pos1 + glm::normalize(delta) * radius;
			Location = glm::vec3(result.x, Location.y, result.y);
		}
		else if (deltaHeight <= halfLength + radius)
		{
			SolveSphereCollision(location + glm::vec3(0.0f, halfLength, 0.0f), radius);
			SolveSphereCollision(location - glm::vec3(0.0f, halfLength, 0.0f), radius);
		}
	}
}

void CClothParticle::Reset()
{
	Location = OriginalLocation;
	OldLocation = Location;
	Force *= 0.0f;
}

void CClothParticle::AddLink(Link* aLink)
{
	Links.push_back(aLink);
}

void CClothParticle::RemoveLink(Link* aLink)
{
	Links.erase(std::find(Links.begin(), Links.end(), aLink));
}

void CClothParticle::ApplyPhysics()
{
	if (glm::length(ForceMultiplier) != 0.0f && Parent->SelfCollisionEnabled)
	{
		float dist = 0.5f;
		float dt = TIMER::GetDeltaSeconds();

		for (CClothParticle* particle : Parent->Particles)
		{
			glm::vec3 delta = Location - particle->Location;
			float deltaLength = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;

			if (deltaLength < dist)
			{
				Location += (delta * (dist - deltaLength) * dt * 5.0f);
			}
		}
	}

	// Apply wind force
	if (Parent->WindData.IsInArea(Location))
	{
		Force += Parent->WindData.WindDirection * Parent->WindData.WindStrength * TIMER::GetDeltaSeconds();
	}


	glm::vec3 delta = Location - OldLocation;
	OldLocation = Location;

	Force += Parent->GetGravity() * TIMER::GetDeltaSeconds();
	Location += (delta * Damping + (Force / Mass) * TIMER::GetDeltaSeconds()) * ForceMultiplier;
	Location.y = std::max(Location.y, Parent->GetGroundHeight() + 0.01f);

	Force *= 0.0f;
}
