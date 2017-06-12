#ifndef _SPHERE_GEOMETRY_
#define _SPHERE_GEOMETRY_

#include "Globals.h"
#include "Materials.h"

class Sphere {
private:
	GLuint VAO, VBO, NBO;
	mat4 model;
	vec3 color;
	Material *mat;
	int stks, slcs;
	vec3 pos;

public:
	Sphere(int, int);
	~Sphere();
	void draw(GLuint);
	void setModel(mat4 m) { model = m; pos = vec3(model[3]); }
	void setMaterial(Material* m) { mat = m; }
	vec3 getPosition() { return pos; }
};

#endif