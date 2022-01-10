#include "scene.h"
#include "room.h"
#include "cameraRotate3D.h"
#include "instanceLayer.h"
#include "timer.h"
#include "soundManager.h"
#include "input.h"
#include "label.h"
#include "font.h"
#include "program.h"
#include "mesh.h"
#include "texture.h"
#include "cloth.h"
#include "guiLayer.h"
#include "button.h"
#include "slider.h"

CScene::CScene(unsigned int scrWidth, unsigned int scrHeight)
{
	ScreenWidth = scrWidth;
	ScreenHeight = scrHeight;

	GroundHeight = nullptr;

	PyramidX = nullptr;
	CapsuleZ = nullptr;
	SphereXZ = nullptr;

	WindAreaScalar = nullptr;

	NumberOfRungs = nullptr;
	RungSeperation = nullptr;
	ClothWidth = nullptr;
	ClothHeight = nullptr;
	
	Floor = nullptr;
	PyramidObj = nullptr;
	CapsuleObj = nullptr;
	SphereObj = nullptr;

	Cloth = nullptr;
}

void CScene::Update()
{
	std::string FPS = std::to_string(1.0f / TIMER::GetDeltaSeconds()).substr(0, 4);
	std::string ConsoleText = "FPS:" + FPS;
	SetConsoleTitle(TEXT(ConsoleText.c_str()));

	TIMER::Update();
	SFX::Update();
	CManager::Update();
	GINPUT::UpdateLocal();

	Floor->Location.y = *GroundHeight;
	PyramidObj->Location.y = *GroundHeight;
	CapsuleObj->Location.y = *GroundHeight + CapsuleObj->Scale.x * 2.0f;
	SphereObj->Location.y = *GroundHeight + SphereObj->Scale.x;

	PyramidObj->Location.z = HELPER::Lerp(40.0f, -40.0f, *PyramidX);

	CapsuleObj->Location.x = HELPER::Lerp(40.0f, -40.0f, *CapsuleZ);
	CapsuleObj->Location.z = HELPER::Lerp(-40.0f, 40.0f, *CapsuleZ);

	SphereObj->Location.x = HELPER::Lerp(-40.0f, 40.0f, *SphereXZ);
	SphereObj->Location.z = HELPER::Lerp(-40.0f, 40.0f, *SphereXZ);

	WindData.Size = glm::vec3(10.0f, 5.0f, 10.0f) * (*WindAreaScalar);

	glutPostRedisplay();
}

void CScene::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffer

	CManager::Render();

	glutSwapBuffers(); // Swap the buffers
	glFinish();
}

void CScene::CleanUp()
{
	CManager::CleanUp();

	delete RungSeperation;
	RungSeperation = nullptr;

	delete GroundHeight;
	GroundHeight = nullptr;

	delete PyramidX;
	PyramidX = nullptr;

	delete CapsuleZ;
	CapsuleZ = nullptr;

	delete SphereXZ;
	SphereXZ = nullptr;

	delete WindAreaScalar;
	WindAreaScalar = nullptr;

	delete NumberOfRungs;
	NumberOfRungs = nullptr;

	delete ClothWidth;
	ClothWidth = nullptr;

	delete ClothHeight;
	ClothHeight = nullptr;
}


void CScene::Initialise()
{
	// Initialise variables
	CManager::Initialise();
	MainCamera = std::shared_ptr<CCameraRotate3D>(new CCameraRotate3D(glm::vec3(), 15.0f));

	WindData.MidPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	WindData.Size = glm::vec3(10.0f, 5.0f, 10.0f);
	WindData.WindDirection = glm::vec3(1.0f, 0.0f, 0.0f);
	WindData.WindStrength = 50.0f;

	// Initialise room
	CRoom* mainRoom = new CRoom(MainCamera);
	Rooms.push_back(mainRoom);

	// Initialise layers
	CInstanceLayer* mainInstanceLayer = new CInstanceLayer();
	mainRoom->Layers.push_back(mainInstanceLayer);

	CGUILayer* mainGUILayer = new CGUILayer();
	mainRoom->Layers.push_back(mainGUILayer);

	// Initialise objects

	GroundHeight = new float(-20.0f);
	mainGUILayer->Objects.push_back(new CLabel(CFont::GetFont("AcknowledgeTTSmall"), "GROUND", glm::vec2(10.0f, 10.0f), glm::ivec3(200.0f, 200.0f, 200.0f)));
	mainGUILayer->Objects.push_back(new CSlider(*GroundHeight, -20.0f, 5.0f, glm::vec2(15.0f, 30.0f), 100.0f));

	PyramidX = new float(0.0f);
	mainGUILayer->Objects.push_back(new CLabel(CFont::GetFont("AcknowledgeTTSmall"), "PYRAMID", glm::vec2(10.0f, 50.0f), glm::ivec3(200.0f, 200.0f, 200.0f)));
	mainGUILayer->Objects.push_back(new CSlider(*PyramidX, 0.0f, 1.0f, glm::vec2(15.0f, 70.0f), 100.0f));

	CapsuleZ = new float(0.0f);
	mainGUILayer->Objects.push_back(new CLabel(CFont::GetFont("AcknowledgeTTSmall"), "CAPSULE", glm::vec2(10.0f, 90.0f), glm::ivec3(200.0f, 200.0f, 200.0f)));
	mainGUILayer->Objects.push_back(new CSlider(*CapsuleZ, 0.0f, 1.0f, glm::vec2(15.0f, 110.0f), 100.0f));

	SphereXZ = new float(0.0f);
	mainGUILayer->Objects.push_back(new CLabel(CFont::GetFont("AcknowledgeTTSmall"), "SPHERE", glm::vec2(10.0f, 130.0f), glm::ivec3(200.0f, 200.0f, 200.0f)));
	mainGUILayer->Objects.push_back(new CSlider(*SphereXZ, 0.0f, 1.0f, glm::vec2(15.0f, 150.0f), 100.0f));

	WindAreaScalar = new float(1.0f);
	mainGUILayer->Objects.push_back(new CLabel(CFont::GetFont("AcknowledgeTTSmall"), "WIND AREA", glm::vec2(10.0f, 170.0f), glm::ivec3(200.0f, 200.0f, 200.0f)));
	mainGUILayer->Objects.push_back(new CSlider(*WindAreaScalar, 0.0f, 2.0f, glm::vec2(15.0f, 190.0f), 100.0f));

	mainGUILayer->Objects.push_back(new CLabel(CFont::GetFont("AcknowledgeTTSmall"), "WIND STRENGTH", glm::vec2(10.0f, 210.0f), glm::ivec3(200.0f, 200.0f, 200.0f)));
	mainGUILayer->Objects.push_back(new CSlider(WindData.WindStrength, 0.0f, 100.0f, glm::vec2(15.0f, 230.0f), 100.0f));



	NumberOfRungs = new float(5.0f);
	mainGUILayer->Objects.push_back(new CLabel(CFont::GetFont("AcknowledgeTTSmall"), "NUM RUNGS", glm::vec2(790.0f, 110.0f), glm::ivec3(200.0f, 200.0f, 200.0f)));
	mainGUILayer->Objects.push_back(new CSlider(*NumberOfRungs, 2.0f, 7.0f, glm::vec2(795.0f, 130.0f), 100.0f));

	RungSeperation = new float(1.0f);
	mainGUILayer->Objects.push_back(new CLabel(CFont::GetFont("AcknowledgeTTSmall"), "RUNGS DIST", glm::vec2(790.0f, 140.0f), glm::ivec3(200.0f, 200.0f, 200.0f)));
	mainGUILayer->Objects.push_back(new CSlider(*RungSeperation, 0.1f, 1.0f, glm::vec2(795.0f, 160.0f), 100.0f));

	ClothWidth = new float(5.0f);
	mainGUILayer->Objects.push_back(new CLabel(CFont::GetFont("AcknowledgeTTSmall"), "CLOTH WIDTH", glm::vec2(790.0f, 40.0f), glm::ivec3(200.0f, 200.0f, 200.0f)));
	mainGUILayer->Objects.push_back(new CSlider(*ClothWidth, 1.0f, 10.0f, glm::vec2(795.0f, 60.0f), 100.0f));

	ClothHeight = new float(5.0f);
	mainGUILayer->Objects.push_back(new CLabel(CFont::GetFont("AcknowledgeTTSmall"), "CLOTH HEIGHT", glm::vec2(790.0f, 70.0f), glm::ivec3(200.0f, 200.0f, 200.0f)));
	mainGUILayer->Objects.push_back(new CSlider(*ClothHeight, 1.0f, 10.0f, glm::vec2(795.0f, 90.0f), 100.0f));

	Cloth = new CCloth(MainCamera, glm::vec3(0.0f, 0.0f, 0.0f), (int)std::roundf(*ClothWidth), (int)std::roundf(*ClothHeight), 1.0f, *NumberOfRungs, *RungSeperation, *GroundHeight, WindData);
	Cloth->SetGravity(glm::vec3(0.0f, -10.0f, 0.0f));
	mainInstanceLayer->Objects.push_back(Cloth);

	PyramidObj = new CObject(CProgram::GetProgram("BLSpecular"), CMesh::GetMesh("NPyramid"));
	MainCamera->AssignCameraPosition(PyramidObj, "camPos");
	PyramidObj->UpdateUniformfVec3("lightPos", glm::vec3(5.0f, 10.0f, 0.0f));
	PyramidObj->UpdateUniformMat4R("model", PyramidObj->GetModelMatrix());
	PyramidObj->UpdateUniformTexture("tex", CTexture::GetTexture("Obstacle")->GetID());
	PyramidObj->Scale *= 6.0f;
	mainInstanceLayer->Objects.push_back(PyramidObj);
	Cloth->SetPyramid(Pyramid{ PyramidObj, PyramidObj->Scale.x });

	CapsuleObj = new CObject(CProgram::GetProgram("BLSpecular"), CMesh::GetMesh("NCapsule"));
	MainCamera->AssignCameraPosition(CapsuleObj, "camPos");
	CapsuleObj->UpdateUniformfVec3("lightPos", glm::vec3(5.0f, 10.0f, 0.0f));
	CapsuleObj->UpdateUniformMat4R("model", CapsuleObj->GetModelMatrix());
	CapsuleObj->UpdateUniformTexture("tex", CTexture::GetTexture("Obstacle")->GetID());
	CapsuleObj->Scale *= 4.0f;
	mainInstanceLayer->Objects.push_back(CapsuleObj);
	Cloth->SetCapsule(Capsule{ CapsuleObj, CapsuleObj->Scale.x, CapsuleObj->Scale.x * 2.0f });

	SphereObj = new CObject(CProgram::GetProgram("BLSpecular"), CMesh::GetMesh("NSphere"));
	MainCamera->AssignCameraPosition(SphereObj, "camPos");
	SphereObj->UpdateUniformfVec3("lightPos", glm::vec3(5.0f, 10.0f, 0.0f));
	SphereObj->UpdateUniformMat4R("model", SphereObj->GetModelMatrix());
	SphereObj->UpdateUniformTexture("tex", CTexture::GetTexture("Obstacle")->GetID());
	SphereObj->Scale *= 4.0f;
	mainInstanceLayer->Objects.push_back(SphereObj);
	Cloth->SetSphere(Sphere{ SphereObj, SphereObj->Scale.x });


	const glm::vec3 colourSpeed = glm::vec3(1.0f, 1.0f, 1.0f) * 1000.0f;

	mainGUILayer->Objects.push_back(new CLabel(CFont::GetFont("AcknowledgeTTSmall"), "GLOBAL WIND", glm::vec2(790.0f, 250.0f), glm::ivec3(200.0f, 200.0f, 200.0f)));
	CButton* globalWindEnabledButton = new CButton(CFont::GetFont("AcknowledgeTTSmall"), "ENABLED", glm::ivec3(255, 255, 255), glm::ivec2(790.0f, 270.0f));
	globalWindEnabledButton->SetOnHover([globalWindEnabledButton, colourSpeed]() { globalWindEnabledButton->SetColour(glm::max(globalWindEnabledButton->GetColour() - colourSpeed * TIMER::GetDeltaSeconds(), glm::vec3(92.0f, 161.0f, 237.0f)));   });
	globalWindEnabledButton->SetOnIdle([globalWindEnabledButton, colourSpeed]() { globalWindEnabledButton->SetColour(glm::min(globalWindEnabledButton->GetColour() + colourSpeed * TIMER::GetDeltaSeconds(), glm::vec3(255.0f, 255.0f, 255.0f))); });
	globalWindEnabledButton->SetOnPress([globalWindEnabledButton, this]() { Cloth->ToggleWind(); globalWindEnabledButton->SetText(Cloth->GetWindEnabled() ? "ENABLED" : "DISABLED"); });
	mainGUILayer->Objects.push_back(globalWindEnabledButton);

	mainGUILayer->Objects.push_back(new CLabel(CFont::GetFont("AcknowledgeTTSmall"), "UNTANGLING", glm::vec2(790.0f, 300.0f), glm::ivec3(200.0f, 200.0f, 200.0f)));
	CButton* selfCollisionEnabledButton = new CButton(CFont::GetFont("AcknowledgeTTSmall"), "DISABLED", glm::ivec3(255, 255, 255), glm::ivec2(790.0f, 320.0f));
	selfCollisionEnabledButton->SetOnHover([selfCollisionEnabledButton, colourSpeed]() { selfCollisionEnabledButton->SetColour(glm::max(selfCollisionEnabledButton->GetColour() - colourSpeed * TIMER::GetDeltaSeconds(), glm::vec3(92.0f, 161.0f, 237.0f)));   });
	selfCollisionEnabledButton->SetOnIdle([selfCollisionEnabledButton, colourSpeed]() { selfCollisionEnabledButton->SetColour(glm::min(selfCollisionEnabledButton->GetColour() + colourSpeed * TIMER::GetDeltaSeconds(), glm::vec3(255.0f, 255.0f, 255.0f))); });
	selfCollisionEnabledButton->SetOnPress([selfCollisionEnabledButton, this]() { Cloth->ToggleSelfCollision(); selfCollisionEnabledButton->SetText(Cloth->GetSelfCollisionEnabled() ? "ENABLED" : "DISABLED"); });
	mainGUILayer->Objects.push_back(selfCollisionEnabledButton);

	CButton* playButton = new CButton(CFont::GetFont("AcknowledgeTTSmall"), "PLAY", glm::ivec3(255, 255, 255), glm::ivec2(790.0f, 10.0f));
	playButton->SetOnHover([playButton, colourSpeed]() { playButton->SetColour(glm::max(playButton->GetColour() - colourSpeed * TIMER::GetDeltaSeconds(), glm::vec3(92.0f, 161.0f, 237.0f)));   });
	playButton->SetOnIdle([playButton, colourSpeed]() { playButton->SetColour(glm::min(playButton->GetColour() + colourSpeed * TIMER::GetDeltaSeconds(), glm::vec3(255.0f, 255.0f, 255.0f))); });
	playButton->SetOnPress([playButton, this]() { Cloth->TogglePlay(); playButton->SetText(Cloth->GetIsPlaying() ? "STOP" : "PLAY"); });
	mainGUILayer->Objects.push_back(playButton);

	CButton* removeRungsButton = new CButton(CFont::GetFont("AcknowledgeTTSmall"), "CLEAR RUNGS", glm::ivec3(255, 255, 255), glm::ivec2(790.0f, 170.0f));
	removeRungsButton->SetOnHover([removeRungsButton, colourSpeed]() { removeRungsButton->SetColour(glm::max(removeRungsButton->GetColour() - colourSpeed * TIMER::GetDeltaSeconds(), glm::vec3(92.0f, 161.0f, 237.0f))); });
	removeRungsButton->SetOnIdle([removeRungsButton, colourSpeed]() { removeRungsButton->SetColour(glm::min(removeRungsButton->GetColour() + colourSpeed * TIMER::GetDeltaSeconds(), glm::vec3(255.0f, 255.0f, 255.0f))); });
	removeRungsButton->SetOnPress([removeRungsButton, this]() { Cloth->ClearRungs(); });
	mainGUILayer->Objects.push_back(removeRungsButton);

	CButton* restartButton = new CButton(CFont::GetFont("AcknowledgeTTSmall"), "RESTART", glm::ivec3(255, 255, 255), glm::ivec2(880.0f, 10.0f));
	restartButton->SetOnHover([restartButton, colourSpeed]() { restartButton->SetColour(glm::max(restartButton->GetColour() - colourSpeed * TIMER::GetDeltaSeconds(), glm::vec3(92.0f, 161.0f, 237.0f))); });
	restartButton->SetOnIdle([restartButton, colourSpeed]() { restartButton->SetColour(glm::min(restartButton->GetColour() + colourSpeed * TIMER::GetDeltaSeconds(), glm::vec3(255.0f, 255.0f, 255.0f))); });
	restartButton->SetOnPress([restartButton, this]() { Cloth->Restart((int)std::roundf(*ClothWidth), (int)std::roundf(*ClothHeight)); });
	mainGUILayer->Objects.push_back(restartButton);

	mainGUILayer->Objects.push_back(new CLabel(CFont::GetFont("AcknowledgeTTSmall"), "RIPPING", glm::vec2(790.0f, 200.0f), glm::ivec3(200.0f, 200.0f, 200.0f)));

	CButton* canRipButton = new CButton(CFont::GetFont("AcknowledgeTTSmall"), "DISABLED", glm::ivec3(255, 255, 255), glm::ivec2(790.0f, 220.0f));
	canRipButton->SetOnHover([canRipButton, colourSpeed]() { canRipButton->SetColour(glm::max(canRipButton->GetColour() - colourSpeed * TIMER::GetDeltaSeconds(), glm::vec3(92.0f, 161.0f, 237.0f)));   });
	canRipButton->SetOnIdle([canRipButton, colourSpeed]() { canRipButton->SetColour(glm::min(canRipButton->GetColour() + colourSpeed * TIMER::GetDeltaSeconds(), glm::vec3(255.0f, 255.0f, 255.0f))); });
	canRipButton->SetOnPress([canRipButton, this]() { Cloth->ToggleRipping(); canRipButton->SetText(Cloth->GetCanRipCloth() ? "ENABLED" : "DISABLED"); });
	mainGUILayer->Objects.push_back(canRipButton);


	Floor = new CObject(CProgram::GetProgram("BLSpecular"), CMesh::GetMesh("3D NSquare"));
	MainCamera->AssignCameraPosition(Floor, "camPos");
	Floor->UpdateUniformfVec3("lightPos", glm::vec3(5.0f, 10.0f, 0.0f));
	Floor->UpdateUniformMat4R("model", Floor->GetModelMatrix());
	Floor->UpdateUniformTexture("tex", CTexture::GetTexture("Floor")->GetID());
	Floor->Scale *= 100;
	Floor->RotationX = 90.0f;
	Floor->Location.y = *GroundHeight;
	mainInstanceLayer->Objects.push_back(Floor);
}