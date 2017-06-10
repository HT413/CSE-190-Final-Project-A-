#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "Globals.h"
#include "OBJObject.h"

enum ACTOR_TYPE{a_Soldier, a_Tank, a_Wall, a_Cannon, a_Tower};

class Actor
{
protected:
	vec3 position;
	mat4 model;
	OBJObject* base;
	bool isActive, isPlacing;
	double lastAction;
	double actionTime;
	double health, damage;
	double range;
	ACTOR_TYPE type;

public:
	Actor(OBJObject*);

	void update();
	void draw(GLuint);
	void setActionTime(double t){ actionTime = t; }
	virtual void move() = 0;
	virtual void doAction() = 0;
	ACTOR_TYPE getType(){ return type; }
};

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