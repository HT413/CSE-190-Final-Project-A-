#ifndef _GRAPHICS_WINDOW_H
#define _GRAPHICS_WINDOW_H

#include "main.h"
extern enum ACTOR_TYPE;

void initObjects();
void destroyObjects();
GLFWwindow* createWindow(int, int);
void resizeCallback(GLFWwindow*, int, int);
void update();
void displayCallback(GLFWwindow*);
void keyCallback(GLFWwindow*, int, int, int, int);
void cursorCallback(GLFWwindow*, double, double);
void mouseCallback(GLFWwindow*, int, int, int);
void scrollCallback(GLFWwindow*, double, double);
void setLeapHand(vec3);
void setLeapOri(vec3);
void setRiftHand(vec3);
void setRiftHead(vec3);
void setRiftHandOri(vec3);
void setRiftHeadOri(vec3);
void createNewUnit(ACTOR_TYPE, int);
void unitPickup(int);
void unitPlacedown();

#endif