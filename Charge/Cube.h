#ifndef _BASIC_CUBE_H_
#define _BASIC_CUBE_H

#include "Globals.h"
#include "OBJObject.h"

const GLfloat vertices[] ={
	// Front
	-.5f, .5f, .5f, -.5f, -.5f, .5f, .5f, -.5f, .5f,
	.5f, -.5f, .5f, .5f, .5f, .5f, -.5f, .5f, .5f,
	// Left
	-.5f, .5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f,
	-.5f, -.5f, .5f, -.5f, .5f, .5f, -.5f, .5f, -.5f,
	// Back
	.5f, .5f, -.5f, .5f, -.5f, -.5f, -.5f, -.5f, -.5f,
	-.5f, -.5f, -.5f, -.5f, .5f, -.5f, .5f, .5f, -.5f,
	// Right
	.5f, .5f, .5f, .5f, -.5f, .5f, .5f, -.5f, -.5f,
	.5f, -.5f, -.5f, .5f, .5f, -.5f, .5f, .5f, .5f,
	// Top
	-.5f, .5f, -.5f, -.5f, .5f, .5f, .5f, .5f, .5f,
	.5f, .5f, .5f, .5f, .5f, -.5f, -.5f, .5f, -.5f,
	// Bottom
	-.5f, -.5f, .5f, -.5f, -.5f, -.5f, .5f, -.5f, -.5f,
	.5f, -.5f, -.5f, .5f, -.5f, .5f, -.5f, -.5f, .5f
};

class Cube
{
private:
	GLuint VAO, VBO;
	mat4 translation, rotation = mat4(1.f);
	vec3 color;

public:
	Cube();
	~Cube();

	void draw(GLuint);
	void setPos(vec3 p) { translation = translate(mat4(1.f), p); }
	void setRotation(mat4 m) { rotation = m; }
	void setColor(vec3 c) { color = c; }
};

#endif