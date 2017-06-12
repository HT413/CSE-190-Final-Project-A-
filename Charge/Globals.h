#ifndef GLOBAL_DECLS
#define GLOBAL_DECLS

#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <Leap.h>

#define WINDOW_TITLE "Final Project"
#define M_PI 3.14159265359f
#define PROGERR(x) getchar(); exit(x);

using namespace std;
using namespace glm;
using namespace Leap;

extern int objCount;
extern bool isGameOver;
extern bool gameStart;
extern Controller controller;

#include "ServerGame.h"
#include "ClientGame.h"
#include <process.h>

extern ServerGame * server;
extern ClientGame * client;

#endif
