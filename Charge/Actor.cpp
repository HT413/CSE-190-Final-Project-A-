#include "Actor.h"

Actor::Actor(OBJObject* obj)
{
	base = obj;
	model = translate(mat4(1.f), vec3(.0f, .0f, 5.f));
	lastAction = glfwGetTime();
	isActive = false;
	isPlacing = true;
}

void Actor::update(){
	double currentTime = glfwGetTime();
	if(!isActive) lastAction = currentTime;
	if((currentTime - lastAction) > actionTime){
		// Do action
		doAction();
	}
	if(isActive)
		move();
}

void Actor::draw(GLuint shaderProgram){
	if(isActive || isPlacing){
		base->setParentModel(model);
		base->draw(shaderProgram);
	}
}

Soldier::Soldier(OBJObject* o) : Actor(o){
	type = a_Soldier;
	health = 250.0;
	damage = 90.0;
	actionTime = 1.2;
	range = 0.5;
}

Cannon::Cannon(OBJObject* o): Actor(o){
	type = a_Cannon;
	health = 160.0;
	damage = 400.0;
	actionTime = 4.0;
	range = 3.0;
}

Tank::Tank(OBJObject* o): Actor(o){
	type = a_Tank;
	health = 1800.0;
	damage = 130.0;
	actionTime = 3.2;
	range = 2.5;
}

Wall::Wall(OBJObject* o): Actor(o){
	type = a_Wall;
	health = 3000.0;
	damage = 0.0;
	actionTime = 999;
	range = 0.0;
}

Tower::Tower(OBJObject* o): Actor(o){
	type = a_Tower;
	health = 10000.0;
	damage = 0.0;
	actionTime = 999;
	range = 0.0;
}

void Soldier::move(){

}

void Soldier::doAction(){

}

void Tank::move(){

}

void Tank::doAction(){

}

void Cannon::move(){

}

void Cannon::doAction(){

}