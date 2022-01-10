#pragma once

#include "object.h"

class CCameraRotate3D;
class CClothParticle;
class CClothMesh;

typedef struct LinkStruct
{
	float RestingDistance;
	float Stiffness;

	CClothParticle* P1;
	CClothParticle* P2;

	LinkStruct(CClothParticle* p1, CClothParticle* p2);
	bool SolveConstraint(bool canBreak);

} Link;

typedef struct SphereStruct
{
	CObject* Obj;
	float Radius;

} Sphere;


typedef struct CapsuleStruct
{
	CObject* Obj;
	float Radius;
	float Height;

} Capsule;


typedef struct PyramidStruct
{
	CObject* Obj;
	float Scale;

} Pyramid;

typedef struct WindAreaStruct
{
	glm::vec3 MidPoint;
	glm::vec3 Size;
	glm::vec3 WindDirection;
	float WindStrength;

	bool IsInArea(glm::vec3 p);
	void Draw();

} WindArea;

class CCloth :
	public CObject
{
public: // Constructors and Destructors
	CCloth(std::shared_ptr<CCameraRotate3D> mainCamera, glm::vec3 location, int width, int height, float spacing, float& numberOfRungs, float& rungSeperation, float& groundHeight, WindArea& windData);
	~CCloth();

public: // Public Functions
	void Update() override;
	void Draw(glm::mat4& pv) override;

	void TogglePlay();
	void ToggleRipping();
	void ToggleWind();
	void ToggleSelfCollision();

	bool GetIsPlaying();
	bool GetCanRipCloth();
	bool GetWindEnabled();
	bool GetSelfCollisionEnabled();

	void ClearRungs();
	void Restart(int width, int height);

	void SetSphere(Sphere sphere);
	void SetPyramid(Pyramid pyramid);
	void SetCapsule(Capsule capsule);

	void SetGravity(glm::vec3 value);
	glm::vec3 GetGravity();

	float GetGroundHeight();

	bool MousePickingParticle(CClothParticle*& particle);
	bool MousePickingLink(Link*& link, int& index);

private: // Private Variables
	friend CClothParticle;

	void RegenerateRungs();
	void RegenerateLinks();

	bool MousePickingPoint(glm::vec3& point);

	std::shared_ptr<CCameraRotate3D> Camera;

	CClothMesh* ClothMesh;
	GLfloat* ClothVertices;
	GLuint ClothVerticesLength;

	std::vector<CClothParticle*> Rungs;
	std::vector<CClothParticle*> Particles;;
	std::vector<Link*> Links;

	Sphere CollisionSphere;
	Capsule CollisionCapsule;
	Pyramid CollisionPyramid;

	WindArea& WindData;

	glm::vec3 Gravity;

	bool IsPlaying;
	bool CanRipCloth;
	bool WindEnabled;
	bool SelfCollisionEnabled;

	float Spacing;
	int Width;
	int Height;

	float& GroundHeight;
	float& RungSeperation;
	float& NumberOfRungs;

	bool IsMovingParticle;
	CClothParticle* MovingParticle;
	glm::vec3 OriginalPoint;
};

