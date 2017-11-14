/*
Shaun McKinnon - 100642799
Selina Daley   - 100558926
Dan Munusami   - 100552012
*/

#define GLEW_STATIC
#include "glew/glew.h"
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "FreeImage.lib")

#include "GameObject.h"
#include "AnimationMath.h"
#include "ParticleEffect.h"
#include "ParticleEmitter.h"
#include "Utility.h"

// Core Libraries (std::)
#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include <vector>

// 3rd Party Libraries
#include <GLUT/freeglut.h>
#include <TTK/GraphicsUtils.h>
#include <TTK/Camera.h>
#include "imgui/imgui.h"


/* GLOBALS */
#define FRAMES_PER_SECOND 60
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND; // milliseconds per frame

// IM stuff
#define IM_ARRAYSIZE(_ARR) ((int) (sizeof(_ARR)/sizeof(*_ARR)))

// Window
int windowWidth = 1200;
int windowHeight = 800;

// Time since last update
float deltaTime = 0.0f;

// Mouse position in pixels
glm::vec3 mousePosition; // x, y, 0
glm::vec3 mousePositionFlipped; // x, windowHeight - y, 0

ParticleEffect particleEffect;
Utility utilities;
std::vector<std::string> fileList; // file list

// Initialize
void Initialize()
{
	// add one particle effect
	ParEmitSettings peSettings;
	//particleEffect.parEmitSettings.push_back(peSettings);

	// file list
	fileList = utilities.fileLister(WORKING_DIRECTORY, "*.dat");
}

// These values are controlled by imgui
bool applySeekingForce = true;
bool magnetActive = false;
float seekingForceScale = 200.0f;
float minSeekingForceScale = -200.0f;
float maxSeekingForceScale = 200.0f;

// PHYSICS //
// Seek
void seek(ParticleEmitter* e, glm::vec3 target)
{
	// TODO: implement seeking
	// Loop through each particle in the emitter and apply a seeking for to them
	for (int i = 0; i < e->getNumParticles(); i++) {
		glm::vec3 seekVector = target - e->getParticlePosition(i);
		glm::vec3 seekDirection = glm::normalize(seekVector);
		glm::vec3 seekForce = seekDirection * seekingForceScale;

		e->applyForceToParticle(i, seekForce);
	}
}

// Flee
void flee(ParticleEmitter* e, glm::vec3 target)
{
	for (int i = 0; i < e->getNumParticles(); i++) {
		glm::vec3 fleeVector = target + e->getParticlePosition(i);
		glm::vec3 fleeDirection = glm::normalize(fleeVector);
		glm::vec3 seekForce = fleeDirection * seekingForceScale;

		e->applyForceToParticle(i, seekForce);
	}
}


// Timer
void TimerCallbackFunction(int value)
{
	// Calculate the amount of time since the last frame
	static int elapsedTimeAtLastTick = 0;
	int totalElapsedTime = glutGet(GLUT_ELAPSED_TIME);

	deltaTime = (float)totalElapsedTime - elapsedTimeAtLastTick;
	deltaTime /= 1000.0f;
	elapsedTimeAtLastTick = totalElapsedTime;

	// Re-trigger the display event
	glutPostRedisplay();

	/* this call gives it a proper frame delay to hit our target FPS */
	glutTimerFunc(FRAME_DELAY, TimerCallbackFunction, 0);
}


// Window Methods
void WindowReshapeCallbackFunction(int w, int h)
{
	// Update our Window Properties
	windowWidth = w;
	windowHeight = h;
}


// Input Methods
void DisplayCallbackFunction(void)
{
	// setting up
	TTK::Graphics::SetBackgroundColour(0.10f, 0.10f, 0.10f);
	TTK::Graphics::ClearScreen();
	TTK::Graphics::SetCameraMode2D(windowWidth, windowHeight);
	// TTK::Graphics::SetCameraMode3D(windowWidth, windowHeight);

	//TTK::Graphics::DrawCube(glm::vec3(200.0f, 300.0f, 0.0f), 35.0f, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	TTK::Graphics::DrawCube(glm::vec3(500, 700, 0), 35.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	// ImGui Start
	TTK::Graphics::StartUI(windowWidth, windowHeight);

	// ImGui Interface Controls
	ImGui::Text("Partice Effect Settings");

	// Input for the particle effect name
	static char peName[255] = "default";
	ImGui::InputText("Particle Effect Name", peName, IM_ARRAYSIZE(peName));
		
	// Saving the particle effect
	if (ImGui::Button("Create/Save Particle Effect"))
	{
		particleEffect.name = peName;

		std::cout << particleEffect.name << " has been saved." << std::endl;
		particleEffect.save();

		// reinit file list
		fileList = utilities.fileLister(WORKING_DIRECTORY, "*.dat");
		particleEffect.setupEmitters();
	}

	ImGui::Separator();

	// loading files
	std::stringstream listStream;
	listStream.write("...New...\0", 10);
	for (int i = 0; i < fileList.size(); i++) {
		listStream.write(&fileList[i][0], fileList[i].size());
		listStream.write("\0", 1);
	}
	static int listIndex = 0;
	static int particleEmitterIndex = 0;


	if (fileList.size() > 0) {
		ImGui::Combo("Load Particle Effect", &listIndex, listStream.str().c_str());
		
		static int lastIndex = 0;
		if (listIndex != lastIndex) {
			particleEmitterIndex = 0;
			lastIndex = listIndex;
			
			//emitter.clear();
			std::cout << "Clearing the emitter" << std::endl;

			if (listIndex != 0) {
				std::string theLoadFile = fileList[listIndex - 1];

				// reset the peName
				memset(peName, 0, 255);
				for (int i = 0; i < (theLoadFile.length() - 4); i++) {
					peName[i] = theLoadFile[i];
				}

				// load the settings
				particleEffect.load(theLoadFile);

				//Set up the emitter
				particleEffect.setupEmitters();
			}
			else
			{
				particleEffect.load("DoNotUseThisFile");
				memset(peName, 0, 255);
				
				std::string theHiddenFileName = "NewFile";
				for (int i = 0; i < theHiddenFileName.length(); i++) {
					peName[i] = theHiddenFileName[i];
				}
			}
		}
	}

	// ImGui for creating new particle emitter
	if (ImGui::Button("Add Particle Emitter"))
	{
		ParEmitSettings newPeSettings;
		particleEffect.parEmitSettings.push_back(newPeSettings);
	}

	ImGui::Separator();

	std::stringstream peIndexList;
	// List of Emitters for this particle effect
	for (int i = 0; i < particleEffect.parEmitSettings.size(); i++)
	{
		peIndexList << i;
		peIndexList.write("\0", 1);
	}


	if (particleEffect.parEmitSettings.size() > 0) {
		ImGui::Combo("Select Particle Emitter", &particleEmitterIndex, peIndexList.str().c_str());

		// Input for the particle effect name	
		ImGui::InputInt("Number of Particles", &particleEffect.parEmitSettings[particleEmitterIndex].numOfPar);
		ImGui::InputFloat3("Position", &particleEffect.parEmitSettings[particleEmitterIndex].position[0]);
		ImGui::InputFloat3("Offset", &particleEffect.parEmitSettings[particleEmitterIndex].offset[0]);
		ImGui::InputFloat3("Velocity", &particleEffect.parEmitSettings[particleEmitterIndex].velocity[0]);
		ImGui::InputFloat3("Acceleration", &particleEffect.parEmitSettings[particleEmitterIndex].acceleration[0]);
		ImGui::InputFloat3("Force", &particleEffect.parEmitSettings[particleEmitterIndex].force[0]);

		ImGui::ColorEdit4("Start Color", &particleEffect.parEmitSettings[particleEmitterIndex].colorStart[0]);
		ImGui::ColorEdit4("End Color", &particleEffect.parEmitSettings[particleEmitterIndex].colorEnd[0]);

		ImGui::SliderFloat2("Mass Range", &particleEffect.parEmitSettings[particleEmitterIndex].mass[0], 0.1, 1.0f);
		ImGui::SliderFloat2("Size Range", &particleEffect.parEmitSettings[particleEmitterIndex].size[0], 0.1f, 30.0f);
		ImGui::SliderFloat2("Life Time Range", &particleEffect.parEmitSettings[particleEmitterIndex].lifetime[0], 10.0f, 50.0f);
		ImGui::SliderFloat2("Rate Range", &particleEffect.parEmitSettings[particleEmitterIndex].rate[0], 20.0f, 1000.0f);
		ImGui::SliderFloat("Duration Range", &particleEffect.parEmitSettings[particleEmitterIndex].duration, 1.0f, 100.0f);

		ImGui::Checkbox("Gravity", &particleEffect.parEmitSettings[particleEmitterIndex].gravity);
		ImGui::Checkbox("Seek", &particleEffect.parEmitSettings[particleEmitterIndex].seekToPoint);
		ImGui::Checkbox("Flee", &particleEffect.parEmitSettings[particleEmitterIndex].fleeFromPoint);
		ImGui::Checkbox("Repel", &particleEffect.parEmitSettings[particleEmitterIndex].repel);
		ImGui::Checkbox("Attract", &particleEffect.parEmitSettings[particleEmitterIndex].attract);
		ImGui::Checkbox("Follow Path", &particleEffect.parEmitSettings[particleEmitterIndex].followPath);
	}
	
	// REAL TIME EMITTER SETTINGS (NOT SAVED TILL YOU SAVE IT)
	if (particleEffect.parEmitSettings.size() > 0) {
		if (particleEffect.emitters.size() > 0) {
			for (int i = 0; i < particleEffect.emitters.size(); i++)
			{
				// Updates emitter position
				particleEffect.emitters[i].emitterPosition = particleEffect.parEmitSettings[i].position;
				// mousePositionFlipped;// particleEffect.parEmitSettings[i].position;

				// Updates colour options
				particleEffect.emitters[i].parEmitSettings.colorStart = particleEffect.parEmitSettings[i].colorStart;
				particleEffect.emitters[i].parEmitSettings.colorEnd   = particleEffect.parEmitSettings[i].colorEnd;

				// Update lifetime
				particleEffect.emitters[i].parEmitSettings.lifetime = particleEffect.parEmitSettings[i].lifetime;
				particleEffect.emitters[i].parEmitSettings.size     = particleEffect.parEmitSettings[i].size;
				particleEffect.emitters[i].parEmitSettings.mass     = particleEffect.parEmitSettings[i].mass;
				

				// PHYSICS METHODS (THESE SHOULD BE MOVED INTO A MORE LOGICAL PLACE) //
				// Seek
				particleEffect.emitters[i].parEmitSettings.seekToPoint = particleEffect.parEmitSettings[i].seekToPoint;
				if (particleEffect.emitters[i].parEmitSettings.seekToPoint) {
					std::cout << "I'M GOING TO FIND YOU" << std::endl;
					seek(&particleEffect.emitters[i], glm::vec3(500, 700, 0));
				}

				// Flee
				particleEffect.emitters[i].parEmitSettings.fleeFromPoint = particleEffect.parEmitSettings[i].fleeFromPoint;
				if (particleEffect.emitters[i].parEmitSettings.fleeFromPoint) {
					std::cout << "RUN AWAY" << std::endl; 
					flee(&particleEffect.emitters[i], glm::vec3(500, 700, 0));
				}

				// Repel
				particleEffect.emitters[i].parEmitSettings.repel = particleEffect.parEmitSettings[i].repel;
				if (particleEffect.emitters[i].parEmitSettings.repel) {
					std::cout << "GET AWAY FROM ME" << std::endl;
					flee(&particleEffect.emitters[i], glm::vec3(mousePositionFlipped.x, mousePositionFlipped.y, 0));
				}

				// Attract
				particleEffect.emitters[i].parEmitSettings.attract = particleEffect.parEmitSettings[i].attract;
				if (particleEffect.emitters[i].parEmitSettings.attract) {
					std::cout << "OHHH MYYY" << std::endl;
					seek(&particleEffect.emitters[i], glm::vec3(mousePositionFlipped.x, mousePositionFlipped.y, 0));
				}

				// Follow Path
				particleEffect.emitters[i].parEmitSettings.followPath = particleEffect.parEmitSettings[i].followPath;
				if (particleEffect.emitters[i].parEmitSettings.followPath) {
					std::cout << "DUM DE DUM" << std::endl;
				}

				// Gravity
				particleEffect.emitters[i].parEmitSettings.gravity = particleEffect.parEmitSettings[i].gravity;
				if (particleEffect.emitters[i].parEmitSettings.gravity) {
					std::cout << "GOING DOWN" << std::endl;
				}


				// update and draw
				particleEffect.emitters[i].update(deltaTime);
				particleEffect.emitters[i].draw();
			}
		}
	}

	// ImGui End and Render
	TTK::Graphics::EndUI();

	glutSwapBuffers();
}

void UpdateEmitters()
{
}

// Keyboard Methods
void KeyboardDownCallbackFunction(unsigned char key, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key] = true;
	io.KeyMap[ImGuiKey_Backspace] = '\b';
	io.AddInputCharacter(key);

	switch (key)
	{
	case 27: // the escape key
		glutExit();
		break;

	case 'q': // the 'q' key
	case 'Q':
		// ...
		break;
	}
}

void KeyboardUpCallbackFunction(unsigned char key, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key] = false;
	io.KeyMap[ImGuiKey_Backspace] = '\b';

	switch (key)
	{
	default:
		break;
	}
}

void SpecialInputCallbackFunction(int key, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[160 + key] = true;
}

void SpecialInputCallbackUpFunction(int key, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[160 + key] = false;
}

// Mouse Methods
void MouseClickCallbackFunction(int button, int state, int x, int y)
{
	ImGui::GetIO().MouseDown[0] = !state;

	mousePosition.x = x;
	mousePosition.y = y;

	mousePositionFlipped.x = x;
	mousePositionFlipped.y = windowHeight - y;
}

void MouseMotionCallbackFunction(int x, int y)
{
	ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);

	mousePosition.x = x;
	mousePosition.y = y;

	mousePositionFlipped.x = x;
	mousePositionFlipped.y = windowHeight - y;
}

void MousePassiveMotionCallbackFunction(int x, int y)
{
	ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);

	mousePositionFlipped.x = x;
	mousePositionFlipped.y = windowHeight - y;
}


// main
int main(int argc, char **argv)
{
	// Init GLUT
	glutInitContextVersion(4, 4);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInit(&argc, argv);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Assignment 02 - Particle Effects");

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "GLEW failed to initialize. You're hopeless." << std::endl;
	}

	// GLUT callbacks
	glutDisplayFunc(DisplayCallbackFunction);
	glutKeyboardFunc(KeyboardDownCallbackFunction);
	glutKeyboardUpFunc(KeyboardUpCallbackFunction);
	glutSpecialFunc(SpecialInputCallbackFunction);
	glutSpecialUpFunc(SpecialInputCallbackUpFunction);
	glutReshapeFunc(WindowReshapeCallbackFunction);
	glutMouseFunc(MouseClickCallbackFunction);
	glutMotionFunc(MouseMotionCallbackFunction);
	glutPassiveMotionFunc(MousePassiveMotionCallbackFunction);
	glutTimerFunc(1, TimerCallbackFunction, 0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// start the game loop
	deltaTime = (float)glutGet(GLUT_ELAPSED_TIME);
	deltaTime /= 1000.0f;

	Initialize();

	// Init ImGui
	TTK::Graphics::InitImGUI();

	glutMainLoop();

	return 0;
}