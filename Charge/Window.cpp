#include "Window.h"
#include "Shader.hpp"
#include "OBJObject.h"
#include "Plane.h"
#include "Skybox.h"
#include "UI_Bar.h"
#include "Actor.h"
#include "Sphere.h"
#include "Cube.h"

#include <Leap.h>
#include <glm/gtx/quaternion.hpp>

#include <FreeImage.h>

// Window width and height
int width, height;
int sessionScreenshots;

// For the trackball
double lastX, lastY;
enum trackballAction { NO_ACTION, C_ROTATE };
trackballAction mouseAction;

// For shader programs
GLuint phongShader, objShader, texShader;
GLuint uiShader, uiRectShader, unitHPShader;

// Light properties
const int MAX_LIGHTS = 8;
int numLights;
float *lightPositions;
float *lightColors;

// Material properties
Material *tank_Green;
vec3 tankAmbient = vec3(.04f, .27f, .19f);
vec3 tankDiffuse = vec3(.22f, .83f, .44f);
vec3 tankSpecular = vec3(.19f, .76f, .40f);
float tankShininess = 5.f;

Material *wall_Brown;
vec3 wallAmbient = vec3(.24f, .19f, .02f);
vec3 wallDiffuse = vec3(.79f, .33f, .05f);
vec3 wallSpecular = vec3(.64f, .36f, .04f);
float wallShininess = 1.f;

Material *soldier_Navy;
vec3 soldierAmbient = vec3(.02f, .19f, .26f);
vec3 soldierDiffuse = vec3(.16f, .51f, .85f);
vec3 soldierSpecular = vec3(.11f, .20f, .80f);
float soldierShininess = 3.f;

Material *cannon_Dark;
vec3 cannonAmbient = vec3(.095f, .065f, .025f);
vec3 cannonDiffuse = vec3(.44f, .36f, .19f);
vec3 cannonSpecular = vec3(.75f, .72f, .35f);
float cannonShininess = 15.f;

Material *castle_Sand;
vec3 castleAmbient = vec3(.19f, .24f, .03f);
vec3 castleDiffuse = vec3(.67f, .84f, .15f);
vec3 castleSpecular = vec3(.75f, .90f, .20f);
float castleShininess = 3.f;

Material *sphere_Green;
vec3 sphereAmbient = vec3(.05f, .29f, .08f);
vec3 sphereDiffuse = vec3(.14f, .93f, .22f);
vec3 sphereSpecular = vec3(.10f, .96f, .15f);
float sphereShininess = 3.f;

Material *sphere_Blue;
vec3 spherebAmbient = vec3(.05f, .08f, .29f);
vec3 spherebDiffuse = vec3(.14f, .22f, .93f);
vec3 spherebSpecular = vec3(.10f, .15f, .96f);

Material *sphere_Red;
vec3 sphererAmbient = vec3(.29f, .08f, .05f);
vec3 sphererDiffuse = vec3(.93f, .22f, .14f);
vec3 sphererSpecular = vec3(.96f, .15f, .10f);

// For the ground
vec3 groundColor = vec3(.6f, .6f, .6f);
Plane *ground;

// For the UI
UI_Bar *selfUI, *foeUI;

// Scene objects
vector<Actor *> selfActors;
vector<Actor *> foeActors;
int objCount = 0;

float selfHP, selfNRG, foeHP;

double lastTime;

// Other variables
vec3 cam_pos(0, 4, 6.5), cam_lookAt(0, 0, 0) , cam_up(0, 1, 0);
mat4 projection, view;
bool gameStart;
double lastUpdateTime;
bool wasPickup;
vec3 handPos = vec3(0.01, 0.01, 4.5);
vec3 handOri = vec3(0, 0, 0);
int objPickup;

// OBJ models
OBJObject* soldierObj, *tankObj, *wallObj, *cannonObj, *castleObj;
Actor* pickedUp;
Cube *handObject, *riftHandObject, *riftHeadObject;
vec3 riftHandPos = vec3(0, -1, 0);
vec3 riftHeadPos = vec3(0, -1, 0);
vec3 riftHandOri = vec3(0, 0, 0);
vec3 riftHeadOri = vec3(0, 0, 0);

const mat4 rShiftMat = translate(mat4(1.f), vec3(0.065, 0, 0));


// Helper func generates random string; len = number of characters, appends ".jpg" to end
void gen_random(char *s, const int len) {
	const char alphanum[] = "0123456789abcdefghijklmnopqrstuvwxyz";

	for(int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	int i = sessionScreenshots;
	int j = 1;
	do{
		s[len - j] = '0' + (i % 10);
		j++;
		i/=10;
	} while(i > 0);
	s[len - j] = '_';
	s[len] = '.';
	s[len + 1] = 'p';
	s[len + 2] = 'n';
	s[len + 3] = 'g';
	s[len + 4] = 0;
}

// Helper func to save a screenshot
void saveScreenshot() {
	int pix = width * height;
	BYTE *pixels = new BYTE[3*pix];
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);

	FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, width, height, width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);

	char *s = new char[15];
	gen_random(s, 10);
	sessionScreenshots++;
	FreeImage_Save(FIF_PNG, img, s, 0);
	cout << "Saved screenshot: " << s << endl;
	delete s;
	delete pixels;
}

GLFWwindow* createWindow(int w, int h){
	// Initialize GLFW
	if(!glfwInit())
	{
		cerr << "Failed to initialize GLFW!" << endl;
		PROGERR(30);
	}

	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(w, h, WINDOW_TITLE, 0, 0);

	// Check if the window could not be created
	if(!window)
	{
		cerr << "Failed to open GLFW window!" << endl;
		glfwTerminate();
		PROGERR(31);
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	resizeCallback(window, width, height);

	return window;
}

void initObjects(){
	srand(rand() % 32768);

	// Create the model
	soldierObj = new OBJObject("objects/soldier.obj");
	tankObj = new OBJObject("objects/tank_T72.obj");
	cannonObj = new OBJObject("objects/cannon.obj");
	wallObj = new OBJObject("objects/wall.obj");
	castleObj = new OBJObject("objects/castle.obj");

	// Lights
	numLights = 2;
	lightPositions = new float[4 * MAX_LIGHTS];
	lightColors = new float[3 * MAX_LIGHTS];
	// Light 0 - directional
	lightPositions[0] = .1f; lightPositions[1] = -.1f; 
	lightPositions[2] = 1.f; lightPositions[3] = 0.f;
	lightColors[0] = 1.f; lightColors[1] = 1.f; lightColors[2] = 1.f;
	// Test light 1 - negation of light 0
	lightPositions[4] = -.1f; lightPositions[5] = -.1f;
	lightPositions[6] = -1.f; lightPositions[7] = 0.f;
	lightColors[3] = 1.f; lightColors[4] = 1.f; lightColors[5] = 1.f;

	// Initialize shaders
	objShader = phongShader = LoadShaders("shaders/basic.vert", "shaders/phong.frag");
	// Phong shading and the regular materials
	glUseProgram(phongShader);
	tank_Green = new RegMaterial();
	((RegMaterial*)tank_Green)->setMaterial(tankAmbient, tankDiffuse, tankSpecular, tankShininess);
	tank_Green->getUniformLocs(phongShader);

	wall_Brown = new RegMaterial();
	((RegMaterial*)wall_Brown)->setMaterial(wallAmbient, wallDiffuse, wallSpecular, wallShininess);
	wall_Brown->getUniformLocs(phongShader);

	soldier_Navy = new RegMaterial();
	((RegMaterial*)soldier_Navy)->setMaterial(soldierAmbient, soldierDiffuse, soldierSpecular, soldierShininess);
	soldier_Navy->getUniformLocs(phongShader);

	cannon_Dark = new RegMaterial();
	((RegMaterial*)cannon_Dark)->setMaterial(cannonAmbient, cannonDiffuse, cannonSpecular, cannonShininess);
	cannon_Dark->getUniformLocs(phongShader);

	castle_Sand = new RegMaterial();
	((RegMaterial*)castle_Sand)->setMaterial(castleAmbient, castleDiffuse, castleSpecular, castleShininess);
	castle_Sand->getUniformLocs(phongShader);

	sphere_Green = new RegMaterial();
	((RegMaterial*)sphere_Green)->setMaterial(sphereAmbient, sphereDiffuse, sphereSpecular, sphereShininess);
	sphere_Green->getUniformLocs(phongShader);

	sphere_Blue = new RegMaterial();
	((RegMaterial*)sphere_Blue)->setMaterial(spherebAmbient, spherebDiffuse, spherebSpecular, sphereShininess);
	sphere_Blue->getUniformLocs(phongShader);

	sphere_Red = new RegMaterial();
	((RegMaterial*)sphere_Red)->setMaterial(sphererAmbient, sphererDiffuse, sphererSpecular, sphereShininess);
	sphere_Red->getUniformLocs(phongShader);

	soldierObj->setMaterial(soldier_Navy);
	soldierObj->setModel(scale(mat4(1.f), vec3(1.2f, 1.f, 1.f)));

	tankObj->setMaterial(tank_Green);
	tankObj->setModel(translate(mat4(1.f), vec3(0, -.25f, 0)) * scale(mat4(1.f), vec3(1.1f, 1.6f, 1.f)));

	cannonObj->setMaterial(cannon_Dark);
	cannonObj->setModel(translate(mat4(1.f), vec3(0, -.25f, 0)) * scale(mat4(1.f), vec3(1.f, 1.2f, 0.9f)) * rotate(mat4(1.f), M_PI, vec3(0, 1, 0)));

	wallObj->setMaterial(wall_Brown);
	wallObj->setModel(translate(mat4(1.f), vec3(0, -.1f, 0)) * scale(mat4(1.f), vec3(1.f, 1.f, 4.f)));

	castleObj->setMaterial(castle_Sand);
	castleObj->setModel(translate(mat4(1.f), vec3(0, 1.3f, -3.f)) * scale(mat4(1.f), vec3(14.f, 5.f, 5.f)));

	// Create towers
	Actor *a1 = new Tower(castleObj);
	objCount++;
	a1->setID(objCount);
	a1->setPosition(0, 0, 5);
	a1->toggleActive();
	a1->togglePlacing();
	a1->setModel(rotate(mat4(1.f), M_PI, vec3(0, 1, 0)));
	selfActors.push_back(a1);

	Actor *a2 = new Tower(castleObj);
	objCount++;
	a2->setID(-objCount);
	a2->setPosition(0, 0, -5);
	a2->toggleActive();
	a2->togglePlacing();
	foeActors.push_back(a2);

	// Create the ground
	texShader = LoadShaders("shaders/texture.vert", "shaders/texture.frag");
	glUseProgram(texShader);
	ground = new Plane(texShader);
	ground->setColor(groundColor);
	ground->setModel(translate(mat4(1), vec3(0, -.5f, 0)) 
		* rotate(mat4(1), M_PI/2.f, vec3(1, 0, 0))
		* scale(mat4(1), vec3(14, 12, 1)));

	// UI shaders and related components
	uiShader = LoadShaders("shaders/ui.vert", "shaders/ui.frag");
	uiRectShader = LoadShaders("shaders/ui_rect.vert", "shaders/ui_rect.frag");
	selfUI = new UI_Bar(true);
	foeUI = new UI_Bar(false);
	selfUI->fetchUniforms(uiShader, uiRectShader);
	foeUI->fetchUniforms(uiShader, uiRectShader);

	// Shaders for the unit HP bars
	unitHPShader = LoadShaders("shaders/unithp.vert", "shaders/unithp.frag");

	// The hand spheres
	glUseProgram(phongShader);
	handObject = new Cube();
	handObject->setColor(vec3(0, 1, 0));
	riftHandObject = new Cube();
	riftHandObject->setColor(vec3(0, 0, 1));
	riftHeadObject = new Cube();
	riftHeadObject->setColor(vec3(1, 0, 0));

	// Misc initializations
	sessionScreenshots = 0;
	selfNRG = 0.f;
	lastUpdateTime = lastTime = glfwGetTime();
	gameStart = false;
	objPickup = 0;
}

void unitPickup(int id) {
	objPickup = id;
	if(id > 0) {
		selfActors[id - 2]->toggleActive();
		selfActors[id - 2]->togglePlacing();
	}
	else {
		foeActors[-2 - id]->toggleActive();
		foeActors[-2 - id]->togglePlacing();
	}
}

void unitPlacedown() {
	if(objPickup > 0) {
		selfActors[objPickup - 2]->toggleActive();
		selfActors[objPickup - 2]->togglePlacing();
	}
	else {
		foeActors[-2 - objPickup]->toggleActive();
		foeActors[-2 - objPickup]->togglePlacing();
	}
	objPickup = 0;
}

void destroyObjects(){
	if(soldierObj) delete soldierObj;
	if(tankObj) delete tankObj;
	if(cannonObj) delete cannonObj;
	if(wallObj) delete wallObj;
	if(castleObj) delete castleObj;
	if(tank_Green) delete tank_Green;
	if(wall_Brown) delete wall_Brown;
	if(soldier_Navy) delete soldier_Navy;
	if(cannon_Dark) delete cannon_Dark;
	if(castle_Sand) delete castle_Sand;
	if(sphere_Green) delete sphere_Green;
	if(sphere_Blue) delete sphere_Blue;
	if(sphere_Red) delete sphere_Red;
	if(handObject) delete handObject;
	if(riftHandObject) delete riftHandObject;
	if(riftHeadObject) delete riftHeadObject;
	if(lightPositions) delete[] lightPositions;
	if(lightColors) delete[] lightColors;
	if(ground) delete ground;
	if(selfUI) delete selfUI;
	if(foeUI) delete foeUI;
	selfActors.clear();
	foeActors.clear();
}

void setRiftHand(vec3 v){
	if(length(v) < .01f) return;
	riftHandPos = v;
}

void setRiftHead(vec3 v){
	if(length(v) < .01f) return;
	riftHeadPos = v;
}

void setRiftHandOri(vec3 v){
	if(length(v) < .01f) return;
	riftHandOri = v;
}

void setRiftHeadOri(vec3 v){
	if(length(v) < .01f) return;
	riftHeadOri = v;
}

void createNewUnit(ACTOR_TYPE type, int id){
	switch(type){
	case a_Soldier:
	{
		Actor* actor = new Soldier(soldierObj);
		actor->setID(id);
		actor->setPosition(riftHandPos.x, 0, riftHandPos.z);
		actor->toggleActive();
		actor->togglePlacing();
		foeActors.push_back(actor);
		break;
	}
	case a_Tank:
	{
		Actor* actor = new Tank(tankObj);
		actor->setID(id);
		actor->setPosition(riftHandPos.x, 0, riftHandPos.z);
		actor->toggleActive();
		actor->togglePlacing();
		foeActors.push_back(actor);
		break;
	}
	case a_Cannon:
	{
		Actor* actor = new Cannon(cannonObj);
		actor->setID(id);
		actor->setPosition(riftHandPos.x, 0, riftHandPos.z);
		actor->toggleActive();
		actor->togglePlacing();
		foeActors.push_back(actor);
		break;
	}
	case a_Wall:
	{
		Actor* actor = new Wall(wallObj);
		actor->setID(id);
		actor->setPosition(riftHandPos.x, 0, riftHandPos.z);
		actor->toggleActive();
		actor->togglePlacing();
		foeActors.push_back(actor);
		break;
	}
	}
}

void resizeCallback(GLFWwindow* window, int w, int h){
	width = w;
	height = h;
	// Set the viewport size
	glViewport(0, 0, width, height);

	if(height > 0)
	{
		projection = perspective(M_PI / 2.f, (float)width / (float)height, 0.1f, 1000.0f);
		view = lookAt(cam_pos, cam_lookAt, cam_up);
	}
}

void setLeapHand(vec3 pos){
	handPos = pos;
}

void setLeapOri(vec3 pos){
	handOri = pos;
}

void update(){
	double currTime = glfwGetTime();
	if(!gameStart) lastTime = currTime;
	if(currTime - lastUpdateTime > 0.04){
		if(client) client->sendHandPos(handPos.x, handPos.y, handPos.z);
		if(client) client->update();
		lastUpdateTime = currTime;
	}

	if(gameStart){
		handObject->setPos(handPos);
		riftHandObject->setPos(riftHandPos);
		riftHeadObject->setPos(riftHeadPos);

		//float wH = sqrtf(1.f - length(handOri));
		//quat leapQuat(wH, handOri.x, handOri.y, handOri.z);
		//handObject->setRotation(toMat4(leapQuat));

		float wL = sqrtf(1.f - length(riftHandOri));
		quat riftHandQuat(wL, riftHandOri.x, riftHandOri.y, riftHandOri.z);
		riftHandObject->setRotation(toMat4(riftHandQuat));

		float wRO = sqrtf(1.f - length(riftHeadOri));
		quat riftHeadQuat(wL, -riftHeadOri.x, -riftHeadOri.y, -riftHeadOri.z);
		riftHeadObject->setRotation(toMat4(riftHeadQuat));

		if(isGameOver) return;
		if(pickedUp)
			pickedUp->setPosition(handPos.x, handPos.y, handPos.z);
		if(objPickup > 0) {
			selfActors[objPickup - 2]->setPosition(riftHandPos.x, riftHandPos.y, riftHandPos.z);
		}
		else if(objPickup < 0){
			foeActors[-2 - objPickup]->setPosition(riftHandPos.x, riftHandPos.y, riftHandPos.z);
		}
		selfNRG += .045f * (currTime - lastTime);
		lastTime = currTime;
		if(selfNRG > 1.f)
			selfNRG = 1.f;
		for(Actor *a : selfActors){
			a->update();
		}
		for(Actor *b : foeActors){
			b->update();
		}
		selfUI->update(selfActors[0]->getHealthRatio(), selfNRG, foeActors[0]->getHealthRatio());
		foeUI->update(selfActors[0]->getHealthRatio(), selfNRG, foeActors[0]->getHealthRatio());
	}
}

void displayCallback(GLFWwindow* window){
	// Draw
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT);

	for(int i = 2; i > 0; i--){
		glColorMask((i == 1)? GL_TRUE : GL_FALSE, GL_TRUE, (i == 1)? GL_FALSE : GL_TRUE, GL_TRUE);
		glClear(GL_DEPTH_BUFFER_BIT);

		glUseProgram(texShader);
		glUniformMatrix4fv(glGetUniformLocation(texShader, "projection"), 1, GL_FALSE, &(projection[0][0]));
		glUniformMatrix4fv(glGetUniformLocation(texShader, "view"), 1, GL_FALSE, &(((i == 1)? view : view * rShiftMat)[0][0]));
		glUniform1i(glGetUniformLocation(texShader, "useMask"), i);
		ground->draw();

		glUseProgram(objShader);
		glUniform1i(glGetUniformLocation(objShader, "numLights"), numLights);
		glUniformMatrix4fv(glGetUniformLocation(objShader, "projection"), 1, GL_FALSE, &(projection[0][0]));
		glUniformMatrix4fv(glGetUniformLocation(objShader, "view"), 1, GL_FALSE, &(((i == 1)? view : view * rShiftMat)[0][0]));
		glUniform3f(glGetUniformLocation(objShader, "camPos"), ((i == 1)? cam_pos : cam_pos)[0], ((i == 1)? cam_pos : cam_pos)[1], ((i == 1)? cam_pos : cam_pos)[2]);
		glUniform4fv(glGetUniformLocation(objShader, "lights"), numLights, lightPositions);
		glUniform3fv(glGetUniformLocation(objShader, "lightCols"), numLights, lightColors);
		glUniform1i(glGetUniformLocation(objShader, "useMask"), i);

		for(Actor *a : selfActors)
			a->draw(objShader);
		for(Actor *b : foeActors)
			b->draw(objShader);

		glUseProgram(unitHPShader);
		glUniformMatrix4fv(glGetUniformLocation(unitHPShader, "projection"), 1, GL_FALSE, &(projection[0][0]));
		glUniformMatrix4fv(glGetUniformLocation(unitHPShader, "view"), 1, GL_FALSE, &(((i == 1)? view : view * rShiftMat)[0][0]));
		for(Actor *a : selfActors)
			a->drawHP(unitHPShader);
		for(Actor *b : foeActors)
			b->drawHP(unitHPShader);
		handObject->draw(unitHPShader);
		riftHandObject->draw(unitHPShader);
		riftHeadObject->draw(unitHPShader);
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	selfUI->draw(uiShader, uiRectShader);
	foeUI->draw(uiShader, uiRectShader);
	glfwSwapBuffers(window);

	glfwPollEvents();
}

// Keyboard callback func
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS) {
		switch(key) {
		// Take a screenshot of the application
		case GLFW_KEY_P:
			saveScreenshot();
			break;

		// Kill the program on pressing Esc
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;

		// Determine pickup or drop for keys 1-5
		case GLFW_KEY_1:
			if(pickedUp) {
				if(pickedUp->getType() == a_Soldier || wasPickup) {
					pickedUp->setPosition(handPos.x, 0, handPos.z);
					pickedUp->togglePlacing();
					pickedUp->toggleActive();
					if(client && !wasPickup){
						client->sendUnitCreation(.1f, pickedUp->getID() + .1f);
					}
					pickedUp = 0;
					wasPickup = false;
				}
				else {
					size_t actorsSize = selfActors.size();
					switch(selfActors[actorsSize - 1]->getType()){
					case a_Soldier: selfNRG += .15f; break;
					case a_Tank: selfNRG += .45f; break;
					case a_Wall: selfNRG += .6f; break;
					case a_Cannon: selfNRG += .5f; break;
					}
					selfActors.erase(selfActors.begin() + actorsSize - 1);
					objCount--;
					pickedUp = 0;
					if(selfNRG >= .15f){
						selfNRG -= .15f;
						pickedUp = new Soldier(soldierObj);
						objCount++;
						pickedUp->setID(objCount);
						pickedUp->setModel(rotate(mat4(1.f), M_PI, vec3(0, 1, 0)));
						selfActors.push_back(pickedUp);
					}
					else{
						cout << "Not enough energy! Cost is 15. You have " << (selfNRG * 100) << endl;
					}
				}
			}
			else {
				if(selfNRG >= .15f){
					selfNRG -= .15f;
					pickedUp = new Soldier(soldierObj);
					objCount++;
					pickedUp->setID(objCount);
					pickedUp->setModel(rotate(mat4(1.f), M_PI, vec3(0, 1, 0)));
					selfActors.push_back(pickedUp);
				}
				else{
					cout << "Not enough energy! Cost is 15. You have " << (selfNRG * 100) << endl;
				}
			}
			break;

		case GLFW_KEY_2:
			if(pickedUp) {
				if(pickedUp->getType() == a_Tank || wasPickup) {
					pickedUp->setPosition(handPos.x, 0, handPos.z);
					pickedUp->togglePlacing();
					pickedUp->toggleActive();
					if(client && !wasPickup){
						client->sendUnitCreation(1.1f, pickedUp->getID() + .1f);
					}
					pickedUp = 0;
					wasPickup = false;
				}
				else {
					size_t actorsSize = selfActors.size();
					switch(selfActors[actorsSize - 1]->getType()){
					case a_Soldier: selfNRG += .15f; break;
					case a_Tank: selfNRG += .45f; break;
					case a_Wall: selfNRG += .6f; break;
					case a_Cannon: selfNRG += .5f; break;
					}
					selfActors.erase(selfActors.begin() + actorsSize - 1);
					objCount--;
					pickedUp = 0;
					if(selfNRG >= .45f){
						selfNRG -= .45f;
						pickedUp = new Tank(tankObj);
						objCount++;
						pickedUp->setID(objCount);
						pickedUp->setModel(rotate(mat4(1.f), M_PI, vec3(0, 1, 0)));
						selfActors.push_back(pickedUp);
					}
					else{
						cout << "Not enough energy! Cost is 45. You have " << (selfNRG * 100) << endl;
					}
				}
			}
			else {
				if(selfNRG >= .45f){
					selfNRG -= .45f;
					pickedUp = new Tank(tankObj);
					objCount++;
					pickedUp->setID(objCount);
					pickedUp->setModel(rotate(mat4(1.f), M_PI, vec3(0, 1, 0)));
					selfActors.push_back(pickedUp);
				}
				else{
					cout << "Not enough energy! Cost is 45. You have " << (selfNRG * 100) << endl;
				}
			}
			break;

		case GLFW_KEY_3:
			if(pickedUp) {
				if(pickedUp->getType() == a_Wall || wasPickup) {
					pickedUp->setPosition(handPos.x, 0, handPos.z);
					pickedUp->togglePlacing();
					pickedUp->toggleActive();
					if(client && !wasPickup){
						client->sendUnitCreation(2.1f, pickedUp->getID() + .1f);
					}
					pickedUp = 0;
					wasPickup = false;
				}
				else {
					size_t actorsSize = selfActors.size();
					switch(selfActors[actorsSize - 1]->getType()){
					case a_Soldier: selfNRG += .15f; break;
					case a_Tank: selfNRG += .45f; break;
					case a_Wall: selfNRG += .6f; break;
					case a_Cannon: selfNRG += .5f; break;
					}
					selfActors.erase(selfActors.begin() + actorsSize - 1);
					objCount--;
					pickedUp = 0;
					if(selfNRG >= .6f){
						selfNRG -= .6f;
						pickedUp = new Wall(wallObj);
						objCount++;
						pickedUp->setID(objCount);
						pickedUp->setModel(rotate(mat4(1.f), M_PI, vec3(0, 1, 0)));
						selfActors.push_back(pickedUp);
					}
					else{
						cout << "Not enough energy! Cost is 60. You have " << (selfNRG * 100) << endl;
					}
				}
			}
			else {
				if(selfNRG >= .6f){
					selfNRG -= .6f;
					pickedUp = new Wall(wallObj);
					objCount++;
					pickedUp->setID(objCount);
					pickedUp->setModel(rotate(mat4(1.f), M_PI, vec3(0, 1, 0)));
					selfActors.push_back(pickedUp);
				}
				else{
					cout << "Not enough energy! Cost is 60. You have " << (selfNRG * 100) << endl;
				}
			}
			break;

		case GLFW_KEY_4:
			if(pickedUp) {
				if(pickedUp->getType() == a_Cannon || wasPickup) {
					pickedUp->setPosition(handPos.x, 0, handPos.z);
					pickedUp->togglePlacing();
					pickedUp->toggleActive();
					if(client && !wasPickup){
						client->sendUnitCreation(3.1f, pickedUp->getID() + .1f);
					}
					pickedUp = 0;
					wasPickup = false;
				}
				else {
					size_t actorsSize = selfActors.size();
					switch(selfActors[actorsSize - 1]->getType()){
					case a_Soldier: selfNRG += .15f; break;
					case a_Tank: selfNRG += .45f; break;
					case a_Wall: selfNRG += .6f; break;
					case a_Cannon: selfNRG += .5f; break;
					}
					selfActors.erase(selfActors.begin() + actorsSize - 1);
					objCount--;
					pickedUp = 0;
					if(selfNRG >= .5f){
						selfNRG -= .5f;
						pickedUp = new Cannon(cannonObj);
						objCount++;
						pickedUp->setID(objCount);
						pickedUp->setModel(rotate(mat4(1.f), M_PI, vec3(0, 1, 0)));
						selfActors.push_back(pickedUp);
					}
					else{
						cout << "Not enough energy! Cost is 50. You have " << (selfNRG * 100) << endl;
					}
				}
			}
			else {
				if(selfNRG >= .5f){
					selfNRG -= .5f;
					pickedUp = new Cannon(cannonObj);
					objCount++;
					pickedUp->setID(objCount);
					pickedUp->setModel(rotate(mat4(1.f), M_PI, vec3(0, 1, 0)));
					selfActors.push_back(pickedUp);
				}
				else{
					cout << "Not enough energy! Cost is 50. You have " << (selfNRG * 100) << endl;
				}
			}
			break;

		case GLFW_KEY_5:
			if(pickedUp) {
				pickedUp->setPosition(handPos.x, 0, handPos.z);
				pickedUp->togglePlacing();
				pickedUp->toggleActive();
				pickedUp = 0;
				client->sendUnitPlaced();
				wasPickup = false;
			}
			// No actor picked up, check if we selected one instead
			else {
				if(selfNRG >= .25f){
					for(Actor *a : selfActors) {
						if(a->getType() == a_Tower) continue;
						if(length(a->getPosition() - handPos) < .5f) {
							pickedUp = a;
							break;
						}
					}
					if(!pickedUp) {
						for(Actor *a : foeActors) {
							if(a->getType() == a_Tower) continue;
							if(length(a->getPosition() - handPos) < .5f) {
								pickedUp = a;
								break;
							}
						}
					}
					if(pickedUp) {
						selfNRG -= .25f;
						pickedUp->toggleActive();
						pickedUp->togglePlacing();
						float theID;
						if(pickedUp->getID() > 0)
							theID = pickedUp->getID() + .1f;
						else
							theID = pickedUp->getID() - .1f;
						client->sendUnitPickup(theID);
						wasPickup = true;
					}
				}
				else{
					cout << "Not enough energy! Cost is 25. You have " << (selfNRG * 100) << endl;
				}
			}
			break;
		}
	}
}

// Mouse cursor callback func
void cursorCallback(GLFWwindow* window, double xpos, double ypos)
{
	// Camera rotation
	if(mouseAction == C_ROTATE) {
		float angle;
		// Perform horizontal (y-axis) rotation
		angle = (float)(lastX - xpos) / 100.0f;
		cam_pos = vec3(rotate(mat4(1.0f), angle, vec3(0.0f, 1.0f, 0.0f)) * vec4(cam_pos, 1.0f));
		cam_up = vec3(rotate(mat4(1.0f), angle, vec3(0.0f, 1.0f, 0.0f)) * vec4(cam_up, 1.0f));
		//Now rotate vertically based on current orientation
		angle = (float)(ypos - lastY) / 100.0f;
		vec3 axis = cross(cam_pos - cam_lookAt, cam_up);
		cam_pos = vec3(rotate(mat4(1.0f), angle, axis) * vec4(cam_pos, 1.0f));
		cam_up = vec3(rotate(mat4(1.0f), angle, axis) * vec4(cam_up, 1.0f));
		// Now update the camera
		view = lookAt(cam_pos, cam_lookAt, cam_up);
		lastX = xpos;
		lastY = ypos;
	}
}

// Mouse button callback func
void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if(action == GLFW_RELEASE) {
		mouseAction = NO_ACTION;
	}
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &lastX, &lastY);
		mouseAction = C_ROTATE;
	}
}

// Scroll wheel callback func
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset){
	cam_pos *= (yOffset > 0)? .99f : 1.01f;
	view = lookAt(cam_pos, cam_lookAt, cam_up);
}