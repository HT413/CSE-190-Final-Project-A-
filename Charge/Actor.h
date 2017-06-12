#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "Globals.h"
#include "OBJObject.h"

enum ACTOR_TYPE{ a_Soldier, a_Tank, a_Wall, a_Cannon, a_Tower };

const GLfloat cubeVerts[] ={
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

class Actor
{
protected:
	GLuint VAO, VBO;
	int id;
	vec3 position;
	mat4 translation, rotation = mat4(1.f);
	OBJObject* base;
	bool isActive, isPlacing, shouldMove;
	double lastAction;
	double actionTime;
	double health, maxHealth, damage;
	double range;
	ACTOR_TYPE type;
	double lastTime, lastRegen;

public:
	Actor(OBJObject*);
	~Actor();

	void update();
	void draw(GLuint);
	void drawHP(GLuint);
	void setActionTime(double t){ actionTime = t; }
	void setID(int i){ id = i; }
	int getID(){ return id; }
	virtual void move() = 0;
	virtual void doAction() = 0;
	ACTOR_TYPE getType(){ return type; }
	void setPosition(float x, float y, float z){
		position = vec3(x, y, z);
		translation = translate(mat4(1.f), position);
	}
	void setModel(mat4 m){
		rotation = m;
	}
	vec3& getPosition(){ return position; }
	bool active(){ return isActive; }
	void toggleActive(){ isActive = !isActive; }
	void togglePlacing(){ isPlacing = !isPlacing; }
	void takeDamage(double amt){
		health -= amt;
		if(type == a_Soldier) cout << "Solider";
		if(type == a_Tank) cout << "Tank";
		if(type == a_Cannon) cout << "Artillery";
		if(type == a_Tower) cout << "Base";
		if(type == a_Wall) cout << "Wall";
		cout << " took " << amt << " damage! Health remaining: " << health << endl;
		if(health < 0) isActive = false;
	}
	double getHealthRatio(){ return health / maxHealth; }
};

extern vector<Actor*> selfActors;
extern vector<Actor*> foeActors;

#endif

#ifndef _SOLDIER_CLASS_
#define _SOLDIER_CLASS_

class Soldier: public Actor{
public:
	Soldier(OBJObject*);
	void move() override;
	void doAction() override;
};

#endif

#ifndef _TANK_CLASS_
#define _TANK_CLASS_

class Tank: public Actor{
public:
	Tank(OBJObject*);
	void move() override;
	void doAction() override;
};

#endif

#ifndef _CANNON_CLASS_
#define _CANNON_CLASS_

class Cannon: public Actor{
public:
	Cannon(OBJObject*);
	void move() override;
	void doAction() override;
};

#endif

#ifndef _WALL_CLASS_
#define _WALL_CLASS_

class Wall: public Actor{
public:
	Wall(OBJObject*);
	void move() override{};
	void doAction() override{};
};

#endif

#ifndef _TOWER_CLASS_
#define _TOWER_CLASS_

class Tower: public Actor{
public:
	Tower(OBJObject*);
	void move() override{};
	void doAction() override{};
};

#endif