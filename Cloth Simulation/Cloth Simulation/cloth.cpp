#include <gtx\intersect.hpp>
#include <iostream>

#include "cloth.h"
#include "clothParticle.h"
#include "clothMesh.h"
#include "input.h"
#include "program.h"
#include "texture.h"
#include "cameraRotate3D.h"
#include "timer.h"
#include "helper.h"

constexpr unsigned SpaceBetweenRungs = 4;

CCloth::CCloth(std::shared_ptr<CCameraRotate3D> mainCamera, glm::vec3 location, int width, int height, float spacing, float& numberOfRungs, float& rungSeperation, float& groundHeight, WindArea& windData) :
	CObject(CProgram::GetProgram("BLSpecular"), ClothMesh = new CClothMesh(Width = width * SpaceBetweenRungs, Height = height * SpaceBetweenRungs, spacing)),
	NumberOfRungs(numberOfRungs),
	RungSeperation(rungSeperation),
	GroundHeight(groundHeight),
	WindData(windData)
{
	mainCamera->AssignCameraPosition(this, "camPos");
	UpdateUniformfVec3("lightPos", glm::vec3(20.0f, 0.0f, 0.0f));
	UpdateUniformFloat("ambientStr", 0.5f);
	UpdateUniformMat4R("model", GetModelMatrix());
	UpdateUniformTexture("tex", CTexture::GetTexture("Cloth")->GetID());

	IsPlaying = false;
	CanRipCloth = false;
	WindEnabled = true;
	SelfCollisionEnabled = false;

	Spacing = spacing;
	Camera = mainCamera;

	glm::vec3 offset = glm::vec3(0.0f, ((float)Height - 1.0f) * spacing * -0.5f, ((float)Width - 1.0f) * spacing * -0.5f);

	// Generate particles
	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			Particles.push_back(new CClothParticle(this, glm::vec3((float)(y - Height) * 0.5f, y * spacing, x * spacing) + offset, { x, y }));
		}
	}
	
	RegenerateRungs();
	RegenerateLinks();
}

CCloth::~CCloth()
{
	for (CClothParticle* aParticle : Particles)
	{
		delete aParticle;
		aParticle = nullptr;
	}

	for (Link* aLink : Links)
	{
		delete aLink;
		aLink = nullptr;
	}

	Particles.clear();
	Links.clear();

	delete Mesh;
	Mesh = nullptr;
}

void CCloth::Update()
{
	if (!MovingParticle && GINPUT::GetRMButtonState() == InputState::INPUT_DOWN)
	{
		Link* link;
		int index;

		if (MousePickingLink(link, index))
		{
			link->P1->RemoveLink(link);
			link->P2->RemoveLink(link);

			ClothMesh->RemoveEdge(link->P1->GetGridLocation(), link->P2->GetGridLocation());

			delete link;
			link = nullptr;

			Links.erase(Links.begin() + index);
		}
	}

	if (IsPlaying)
	{
		if (!IsMovingParticle)
		{
			CClothParticle* particle;

			if (GINPUT::GetLMButtonState() == InputState::INPUT_DOWN_FIRST && MousePickingParticle(MovingParticle) && glm::length(MovingParticle->GetForceMultiplier()) != 0.0f)
			{
				MovingParticle->SetForceMultiplier(0.0f, 0.0f, 0.0f);
				OriginalPoint = MovingParticle->Location;
				IsMovingParticle = true;
			}
			else if (GINPUT::GetKeyInputState(' ') == InputState::INPUT_DOWN_FIRST && MousePickingParticle(particle))
			{
				particle->SetAflame();
			}
		}
		else
		{
			if (GINPUT::GetLMButtonState() == InputState::INPUT_UP_FIRST)
			{
				MovingParticle->SetForceMultiplier(1.0f, 1.0f, 1.0f);
				IsMovingParticle = false;
				OriginalPoint = glm::vec3();
				MovingParticle = nullptr;
			}
			else
			{
				// Get the ray
				glm::vec3 pos = Camera->GetCamPos();
				glm::vec3 dir = Camera->GetRayFromMouse();

				// Get Plane Normal
				glm::vec3 norm = Camera->GetCamPos() - Camera->GetCamLookPos();

				float dist;

				if (glm::intersectRayPlane(pos, dir, OriginalPoint, norm, dist))
				{
					glm::vec3 targetPoint = pos + dir * dist;
					MovingParticle->Location = targetPoint;
				}
			}
		}

		// Wind
		Gravity.x = WindEnabled ? sinf(TIMER::GetTotalSeconds()) * 5.0f : 0.0f;

		for (CClothParticle* aParticle : Rungs)
		{
			aParticle->Location.z = aParticle->GetOriginalLocation().z * RungSeperation;
		}

		for (CClothParticle* aParticle : Particles)
		{
			aParticle->Update();
		}

		std::vector<Link*>::iterator it = Links.begin();
		while (it != Links.end())
		{
			if (((*it)->P1->GetFireTick() == 1.0f && (*it)->P2->GetFireTick() == 1.0f) || !(*it)->SolveConstraint(CanRipCloth))
			{
				(*it)->P1->RemoveLink(*it);
				(*it)->P2->RemoveLink(*it);

				ClothMesh->RemoveEdge((*it)->P1->GetGridLocation(), (*it)->P2->GetGridLocation());
				
				delete (*it);
				it = Links.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			CClothParticle* aParticle = Particles[y * Width + x];

			if (IsPlaying && glm::length(aParticle->GetForceMultiplier()) != 0.0f)
			{
				IfThenStatement(CollisionSphere.Obj != nullptr, aParticle->SolveSphereCollision(CollisionSphere.Obj->Location, CollisionSphere.Radius));
				IfThenStatement(CollisionPyramid.Obj != nullptr, aParticle->SolvePyramidCollision(CollisionPyramid.Obj->Location, CollisionPyramid.Scale));
				IfThenStatement(CollisionCapsule.Obj != nullptr, aParticle->SolveCapsuleCollision(CollisionCapsule.Obj->Location, CollisionCapsule.Height, CollisionCapsule.Radius));
			}

			ClothMesh->UpdatePoint(x, y, aParticle->Location);
		}
	}

	ClothMesh->UpdateMesh();
	CObject::Update();
}

void CCloth::Draw(glm::mat4& pv)
{
	CObject::Draw(pv);

	WindData.Draw();

	glBegin(GL_LINES);

	for (Link* link : Links)
	{
		glm::vec4 p1 = CCamera::GetProjViewMatrix() * glm::vec4(link->P1->Location, 1.0f);
		glm::vec4 p2 = CCamera::GetProjViewMatrix() * glm::vec4(link->P2->Location, 1.0f);

		glColor3f(link->P1->GetFireTick() , 0.0f, 0.0f);
		glVertex3f(p1.x / p1.w, p1.y / p1.w, p1.z / p1.w - 0.00001f);
		
		glColor3f(link->P2->GetFireTick(), 0.0f, 0.0f);
		glVertex3f(p2.x / p2.w, p2.y / p2.w, p2.z / p2.w - 0.00001f);

	}

	glEnd();

	glColor3f(0.0f, 0.0f, 0.0f);
}

void CCloth::ToggleRipping()
{
	CanRipCloth = !CanRipCloth;
}

void CCloth::ToggleWind()
{
	WindEnabled = !WindEnabled;
}

void CCloth::ToggleSelfCollision()
{
	SelfCollisionEnabled = !SelfCollisionEnabled;
}

void CCloth::TogglePlay()
{
	IsPlaying = !IsPlaying;
}

bool CCloth::GetIsPlaying()
{
	return IsPlaying;
}

bool CCloth::GetCanRipCloth()
{
	return CanRipCloth;
}

bool CCloth::GetWindEnabled()
{
	return WindEnabled;
}

bool CCloth::GetSelfCollisionEnabled()
{
	return 	SelfCollisionEnabled;
}

void CCloth::SetGravity(glm::vec3 value)
{
	Gravity = value;
}

glm::vec3 CCloth::GetGravity()
{
	return Gravity;
}

float CCloth::GetGroundHeight()
{
	return GroundHeight;
}

bool CCloth::MousePickingParticle(CClothParticle*& particle)
{
	// Get the ray
	glm::vec3 origin = Camera->GetCamPos();
	glm::vec3 dir = Camera->GetRayFromMouse();


	particle = nullptr;
	float closestDistance = 1.8f;

	for (unsigned int i = 0; i < Particles.size(); ++i)
	{
		glm::vec3 p1 = Particles[i]->Location;

		float dist = glm::length(glm::cross(dir, p1 - origin));
		if (dist < closestDistance)
		{
			particle = Particles[i];
			closestDistance = dist;
		}
	}

	return particle != nullptr;
}

bool CCloth::MousePickingLink(Link*& link, int& index)
{
	// Get the ray
	glm::vec3 origin = Camera->GetCamPos();
	glm::vec3 dir = Camera->GetRayFromMouse();


	index = -1;
	link = nullptr;
	float closestDistance = 1.0f;

	for (unsigned int i = 0; i < Links.size(); ++i)
	{
		glm::vec3 p1 = Links[i]->P1->Location;
		glm::vec3 p2 = Links[i]->P2->Location;

		glm::vec3 midpoint = p1 + (p2 - p1) * 0.5f;

		float dist = glm::length(glm::cross(dir, midpoint - origin));

		if (dist < closestDistance)
		{
			index = i;
			link = Links[i];
			closestDistance = dist;
		}
	}

	return link != nullptr;
}

void CCloth::RegenerateRungs()
{
	Rungs.clear();

	assert(std::roundf(NumberOfRungs) >= 2.0f);
	const int Splits = (int)(std::roundf(NumberOfRungs) - 2);

	// Set left most and right most to static:

	// Left most particle
	CClothParticle* leftParticle = Particles[(Height - 1) * Width];
	leftParticle->SetForceMultiplier(0.0f, 0.0f, 0.0f);
	Rungs.push_back(leftParticle);

	// Right most particle
	CClothParticle* rightParticle = Particles[(Height - 1) * Width + Width - 1];
	rightParticle->SetForceMultiplier(0.0f, 0.0f, 0.0f);
	Rungs.push_back(rightParticle);


	// Split the rest evenly across the cloth

	// Set rungs
	float start = float((Height - 1) * Width);
	float inc = float(Width - 1) / float(Splits + 1);

	for (int i = 1; i <= Splits; ++i)
	{
		int index = (int)std::round(start + inc * i);

		CClothParticle* particle = Particles[index];
		particle->SetForceMultiplier(0.0f, 0.0f, 0.0f);
		Rungs.push_back(particle);
	}
}

void CCloth::RegenerateLinks()
{
	while (!Links.empty())
	{
		delete* (Links.end() - 1);
		Links.pop_back();
	}

	// Generate horizontal links
	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width - 1; ++x)
		{
			CClothParticle* p1 = Particles[y * Width + x];
			CClothParticle* p2 = Particles[y * Width + x + 1];

			Link* link = new Link(p1, p2);
			Links.push_back(link);

			p1->AddLink(link);
			p2->AddLink(link);
		}
	}

	// Generate vertical links
	for (int x = 0; x < Width; ++x)
	{
		for (int y = 0; y < Height - 1; ++y)
		{
			CClothParticle* p1 = Particles[y * Width + x];
			CClothParticle* p2 = Particles[(y + 1) * Width + x];

			Link* link = new Link(p1, p2);
			Links.push_back(link);

			p1->AddLink(link);
			p2->AddLink(link);
		}
	}

	// Generate diagonal links
	for (int x = 0; x < Width - 1; ++x)
	{
		for (int y = 0; y < Height - 1; ++y)
		{
			CClothParticle* p1 = Particles[y * Width + x + 1];
			CClothParticle* p2 = Particles[(y + 1) * Width + x];

			Link* link1 = new Link(p1, p2);
			Links.push_back(link1);

			p1->AddLink(link1);
			p2->AddLink(link1);

			CClothParticle* p3 = Particles[y * Width + x];
			CClothParticle* p4 = Particles[(y + 1) * Width + x + 1];

			Link* link2 = new Link(p3, p4);
			Links.push_back(link2);

			p3->AddLink(link2);
			p4->AddLink(link2);
		}
	}
}

bool CCloth::MousePickingPoint(glm::vec3& point)
{
	point = glm::vec3(0.0f, 0.0f, 0.0f);

	// Get the ray
	glm::vec3 pos = Camera->GetCamPos();
	glm::vec3 dir = Camera->GetRayFromMouse();

	// Get mesh vertice and indice data
	int BufferLength = ClothMesh->GetBufferLength();
	GLfloat* BufferData = ClothMesh->GetBufferData();

	int IndiceLength = ClothMesh->GetIndiceLength();
	GLuint* IndiceData = ClothMesh->GetIndiceData();

	// List of vertices (split off from all the buffer data)
	std::vector<GLfloat> verts;


	for (int i = 0; i < (int)BufferLength; i += 8)
	{
		// Get the point of the mesh
		glm::vec4 pt = glm::vec4(BufferData[i], BufferData[i + 1], BufferData[i + 2], 1.0f);

		// Multiply by the model matrix to get it's in world position
		pt = ModelMatrix * pt;

		// Save for the indice check
		verts.push_back(pt.x);
		verts.push_back(pt.y);
		verts.push_back(pt.z);
	}

	glm::vec3 hitPoint;

	// Loop through the indices
	for (int i = 0; i < IndiceLength; i += 3)
	{
		// Get indices
		int index1 = IndiceData[i] * 3;
		int index2 = IndiceData[i + 1] * 3;
		int index3 = IndiceData[i + 2] * 3;

		// Get the vertices to make the given triangle
		glm::vec3 vec1 = glm::vec3(verts[index1], verts[index1 + 1], verts[index1 + 2]);
		glm::vec3 vec2 = glm::vec3(verts[index2], verts[index2 + 1], verts[index2 + 2]);
		glm::vec3 vec3 = glm::vec3(verts[index3], verts[index3 + 1], verts[index3 + 2]);

		// Check for an intersection with the ray
		if (glm::intersectRayTriangle(pos, dir, vec1, vec2, vec3, hitPoint) ||
			glm::intersectRayTriangle(pos, dir, vec1, vec3, vec2, hitPoint))
		{
			point = pos + dir * hitPoint.z;
			return true;
		}
	}

	return false;
}

void CCloth::ClearRungs()
{
	for (CClothParticle* particle : Rungs)
	{
		particle->SetForceMultiplier(1.0f, 1.0f, 1.0f);
	}

	Rungs.clear();
}

void CCloth::Restart(int width, int height)
{
	Width = width * SpaceBetweenRungs;
	Height = height * SpaceBetweenRungs;

	while (!Particles.empty())
	{
		CClothParticle* p = Particles.back();
		Particles.pop_back();
		delete p;
	}

	glm::vec3 offset = glm::vec3(0.0f, ((float)Height - 1.0f) * Spacing * -0.5f, ((float)Width - 1.0f) * Spacing * -0.5f);

	// Generate particles
	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			Particles.push_back(new CClothParticle(this, glm::vec3((float)(y - Height) * 0.5f, y * Spacing, x * Spacing) + offset, { x, y }));
		}
	}
	
	RegenerateRungs();
	RegenerateLinks();
	ClothMesh->ResetMesh();
}

void CCloth::SetSphere(Sphere sphere)
{
	CollisionSphere = sphere;
}

void CCloth::SetPyramid(Pyramid pyramid)
{
	CollisionPyramid = pyramid;
}

void CCloth::SetCapsule(Capsule capsule)
{
	CollisionCapsule = capsule;
}

LinkStruct::LinkStruct(CClothParticle* p1, CClothParticle* p2)
{
	P1 = p1;
	P2 = p2;

	Stiffness = 0.9f;
	RestingDistance = glm::length(P1->Location - P2->Location);
}

constexpr float RipThreshold = 2.0f;
bool LinkStruct::SolveConstraint(bool canBreak)
{
	glm::vec3 delta = P2->Location - P1->Location;
	float deltaLength = glm::length(delta);

	if (canBreak && deltaLength - RestingDistance > RipThreshold)
	{
		return false;
	}
	else
	{
		float difference = (deltaLength - RestingDistance) / deltaLength;

		float lm1 = 1.0f / P1->GetMass();
		float lm2 = 1.0f / P2->GetMass();

		P1->Move(delta * (lm1 / (lm1 + lm2)) * Stiffness * difference);
		P2->Move(-delta * (lm2 / (lm1 + lm2)) * Stiffness * difference);

		return true;
	}
}

bool WindAreaStruct::IsInArea(glm::vec3 p)
{
	glm::vec3 delta = p - MidPoint;
	glm::bvec3 IsInside = glm::lessThanEqual(glm::abs(delta), (Size * 0.5f));
	return IsInside.x && IsInside.y && IsInside.z;
}

void WindAreaStruct::Draw()
{
	glColor3f(0.0f, 0.0f, 1.0f);

	glm::vec3 p1 = MidPoint - Size * 0.5f;
	glm::vec3 p2 = MidPoint + Size * 0.5f;

	std::vector<glm::vec3> points;

	points.push_back(p1);
	points.push_back(glm::vec3(p1.x, p1.y, p2.z));
	points.push_back(glm::vec3(p1.x, p2.y, p2.z));
	points.push_back(glm::vec3(p1.x, p2.y, p1.z));
	points.push_back(p1);

	points.push_back(glm::vec3(p2.x, p1.y, p1.z));
	points.push_back(glm::vec3(p2.x, p1.y, p2.z));
	points.push_back(p2);
	points.push_back(glm::vec3(p2.x, p2.y, p1.z));
	points.push_back(glm::vec3(p2.x, p1.y, p1.z));

	glBegin(GL_LINE_STRIP);

	for (unsigned int i = 0; i < points.size(); ++i)
	{
		// World space
		glm::vec3 w1 = points[i];

		// Clip space
		glm::vec4 point = CCamera::GetProjViewMatrix() * glm::vec4(w1.x, w1.y, w1.z, 1.0f);

		// Screen space
		glVertex3f(point.x / point.w, point.y / point.w, point.z / point.w);
	}

	glEnd();


	points.clear();

	points.push_back(glm::vec3(p2.x, p1.y, p2.z));
	points.push_back(glm::vec3(p1.x, p1.y, p2.z));

	points.push_back(glm::vec3(p2.x, p2.y, p2.z));
	points.push_back(glm::vec3(p1.x, p2.y, p2.z));

	points.push_back(glm::vec3(p1.x, p2.y, p1.z));
	points.push_back(glm::vec3(p2.x, p2.y, p1.z));

	glBegin(GL_LINES);

	for (unsigned int i = 0; i < points.size(); ++i)
	{
		glm::vec3 w = points[i];

		// Clip space
		glm::vec4 point = CCamera::GetProjViewMatrix() * glm::vec4(w.x, w.y, w.z, 1.0f);

		// Screen space
		glVertex3f(point.x / point.w, point.y / point.w, point.z / point.w);
	}

	glEnd();

	glColor3f(0.0f, 0.0f, 0.0f);
}

