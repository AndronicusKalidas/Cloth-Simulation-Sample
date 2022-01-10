#pragma once
#include <iostream>

#include "manager.h"
#include "cloth.h"

class CInstanceLayer;
class CCameraRotate3D;
class CObject;

class CScene :
	public CManager
{
public:
	std::shared_ptr<CCameraRotate3D> MainCamera;

	CScene(unsigned int scrWidth, unsigned int scrHeight);

	void Initialise() override;
	void Update() override;
	void Render() override;
	void CleanUp() override;


private:

	float* GroundHeight;

	float* PyramidX;
	float* CapsuleZ;
	float* SphereXZ;

	float* WindAreaScalar;
	
	float* NumberOfRungs;
	float* RungSeperation;
	float* ClothWidth;
	float* ClothHeight;

	CObject* Floor;
	CObject* PyramidObj;
	CObject* CapsuleObj;
	CObject* SphereObj;

	CCloth* Cloth;

	WindArea WindData;
};