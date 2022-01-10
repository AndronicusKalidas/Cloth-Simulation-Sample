#pragma once

#include "object.h"
#include "cloth.h"

class CClothParticle
{
public: // Constructors and Destructors
	CClothParticle(CCloth* parent, glm::vec3 location, glm::ivec2 gridLoc);
	~CClothParticle();

public: // Public Functions
	void Update();

	float GetMass();
	void Move(glm::vec3 delta);
	
	void SetForceMultiplier(float x, float y, float z);
	glm::vec3 GetForceMultiplier();

	glm::vec3 GetOriginalLocation();
	glm::ivec2 GetGridLocation();

	float GetFireTick();
	void SetAflame();

	void SolveSphereCollision(glm::vec3 location, float radius);
	void SolvePyramidCollision(glm::vec3 location, float scale);
	void SolveCapsuleCollision(glm::vec3 location, float length, float radius);

	void Reset();

	void AddLink(Link* aLink);
	void RemoveLink(Link* aLink);

public: // Public Variables
	glm::vec3 Location;

private: // Private Functions
	void ApplyPhysics();

private: // Private Variables
	CCloth* Parent;
	
	std::vector<Link*> Links;
	
	glm::vec3 ForceMultiplier;

	float Mass;
	float Damping;

	glm::vec3 Force;
	glm::vec3 OldLocation;
	glm::vec3 OriginalLocation;
	glm::ivec2 GridLocation;

	bool IsOnFire;
	float FireTick;

	float FireTickToBurn = 3.0f;
	float FireTickToSpread = 0.5f;
};

