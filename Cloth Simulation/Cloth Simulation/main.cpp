#include <fmod.hpp>
#include <iostream>
#include <chrono>
#include <fstream>
#include <vld.h>

#include "program.h"
#include "object.h"
#include "helper.h"
#include "sprite.h"
#include "camera.h"
#include "input.h"
#include "scene.h"
#include "mesh.h"
#include "soundManager.h"
#include "timer.h"

void Update();
void Render();
void CleanUp();
void KeyDown(unsigned char key, int x, int y);
void KeyUp(unsigned char key, int x, int y);
void SpecialKeyDown(int key, int x, int y);
void SpecialKeyUp(int key, int x, int y);
void MouseClick(int button, int state, int x, int y);
void MousePassiveMove(int x, int y);
void MouseMove(int x, int y);

const unsigned int ScreenWidth = 1000;
const unsigned int ScreenHeight = 1000;

CScene* MainScene;

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
}

int main(int argc, char** argv)
{
	//Setup and create at glut controlled window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 0);
	glutInitWindowSize(ScreenWidth, ScreenHeight);
	glutCreateWindow("Cloth Simulation");

	// Sets up all gl funcion callbacks based on pc hardware
	if (glewInit() != GLEW_OK)
	{
		// If glew setup failed then applcation will not run graphics correctly
		std::cout << "Glew Initialisation Failed. Aborting Application." << std::endl;
		system("pause");
	}

	glClearColor(0.5, 0.5, 0.5, 1.0);

	//glCullFace(GL_BACK); // Cull the back facing triangles
	//glFrontFace(GL_CCW); // Front facing triangles are drawn in Counter Clockwise
	//glEnable(GL_CULL_FACE); // Enable culling

	// Enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// enable depth for 3d
	glEnable(GL_DEPTH_TEST);

	// enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	MainScene = new CScene(ScreenWidth, ScreenHeight);
	MainScene->Initialise();

	// Register callbacks
	glutDisplayFunc(Render);
	glutIdleFunc(Update);
	glutCloseFunc(CleanUp);
	glutKeyboardFunc(KeyDown);
	glutKeyboardUpFunc(KeyUp);
	glutSpecialFunc(SpecialKeyDown);
	glutSpecialUpFunc(SpecialKeyUp);
	glutMouseFunc(MouseClick);
	glutMotionFunc(GINPUT::MouseMove);
	glutPassiveMotionFunc(GINPUT::MouseMove);

	glutMainLoop(); // Ensure this is the last glut line called
	return 0;
}

void Update()
{
	MainScene->Update();

	std::string FPS = std::to_string(1.0f / TIMER::GetDeltaSeconds()).substr(0, 6);
	std::string ConsoleText = "FPS:" + FPS;
	SetConsoleTitle(TEXT(ConsoleText.c_str()));
}

void Render()
{
	MainScene->Render();
}

void CleanUp()
{
	MainScene->CleanUp();
	delete MainScene;
}

void KeyDown(unsigned char key, int x, int y)
{
	GINPUT::KeyDown(key);
}

void KeyUp(unsigned char key, int x, int y)
{
	GINPUT::KeyUp(key);
}

void SpecialKeyDown(int key, int x, int y)
{
	GINPUT::SpecialKeyDown(key);
}

void SpecialKeyUp(int key, int x, int y)
{
	GINPUT::SpecialKeyUp(key);
}

void MouseClick(int button, int state, int x, int y)
{
	GINPUT::MouseClick(button, state);
}

void MousePassiveMove(int x, int y)
{
	GINPUT::MouseMove(x, y);
}

void MouseMove(int x, int y)
{
	GINPUT::MouseMove(x, y);
}

