#include "Actor.h"

Actor::Actor(OBJObject* obj)
{
	base = obj;
	lastRegen = lastTime = lastAction = glfwGetTime();
	isActive = false;
	isPlacing = true;
	shouldMove = false;

	// Generate a VAO and VBO for the unit HP bars
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindVertexArray(0);
}


Actor::~Actor() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}


void Actor::update(){
	double currentTime = glfwGetTime();
	if(!isActive) lastAction = currentTime;
	if(!shouldMove && (currentTime - lastAction) > actionTime){
		// Do action
		doAction();
	}
	if(isActive){
		health += 6.0 * (currentTime - lastRegen);
		lastRegen = currentTime;
		if(health > maxHealth)
			health = maxHealth;
		move();
	}
}

void Actor::draw(GLuint shaderProgram){
	if(isActive || isPlacing){
		translation = translate(mat4(1.f), position);
		base->setParentModel(translation * rotation);
		base->draw(shaderProgram);
	}
}

void Actor::drawHP(GLuint shaderProgram) {
	if(type == a_Tower || health < 0) return;
	glUniform3f(glGetUniformLocation(shaderProgram, "color"), 0.f, 1.f, 0.f);
	mat4 hpMat = translate(mat4(1.f), vec3(0.f, .7f, 0.f)) * translation * scale(mat4(1.f), vec3((health / 500.f), .1f, .1f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &(hpMat[0][0]));

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

Soldier::Soldier(OBJObject* o) : Actor(o){
	type = a_Soldier;
	maxHealth = health = 270.0;
	damage = 80.0;
	actionTime = 1.2;
	range = 0.5;
}

Cannon::Cannon(OBJObject* o): Actor(o){
	type = a_Cannon;
	maxHealth = health = 160.0;
	damage = 400.0;
	actionTime = 4.0;
	range = 3.0;
}

Tank::Tank(OBJObject* o): Actor(o){
	type = a_Tank;
	maxHealth = health = 1800.0;
	damage = 160.0;
	actionTime = 3.2;
	range = 2.5;
}

Wall::Wall(OBJObject* o): Actor(o){
	type = a_Wall;
	maxHealth = health = 3000.0;
	damage = 0.0;
	actionTime = 999;
	range = 0.0;
}

Tower::Tower(OBJObject* o): Actor(o){
	type = a_Tower;
	maxHealth = health = 10000.0;
	damage = 0.0;
	actionTime = 999;
	range = 0.0;
}

void Soldier::move(){
	// Check if should move
	double currTime = glfwGetTime();
	if(!isActive) lastTime = currTime;
	Actor *collideAgainst = 0;
	if(shouldMove){
		position.z += ((id > 0)? -1.8f : 1.8f) * (currTime - lastTime);
		lastTime = currTime;
		// Check which opponent (if any) are in this unit's hit range
		for(Actor *a : (id > 0)? foeActors : selfActors){
			if(!a->active()) continue;
			vec3& foePos = a->getPosition();
			if(fabs(position.x - foePos.x) > .25f && a->getType() != a_Tower) continue;
			if(abs(position.z - foePos.z) > range) continue;
			// Found a unit in hit range, stop moving to prepare for attack
			shouldMove = false;
			lastAction = glfwGetTime();
			break;
		}
	}
	else{
		// Check which opponent (if any) are in this unit's hit range
		for(Actor *a : (id > 0)? foeActors : selfActors){
			if(!a->active()) continue;
			vec3& foePos = a->getPosition();
			if(fabs(position.x - foePos.x) > .25f && a->getType() != a_Tower) continue;
			if(abs(position.z - foePos.z) > range) continue;
			// Found a unit in hit range, remain immobile
			collideAgainst = a;
			break;
		}
		// No units found, can move again
		if(!collideAgainst){
			lastTime = glfwGetTime();
			shouldMove = true;
		}
	}
}

void Soldier::doAction(){
	Actor *collideAgainst = 0;
	float distance = 10;
	if(!shouldMove){
		// Check which opponent (if any) are in this unit's hit range
		for(Actor *a : (id > 0)? foeActors : selfActors){
			if(!a->active()) continue;
			vec3& foePos = a->getPosition();
			if(fabs(position.x - foePos.x) > .25f && a->getType() != a_Tower) continue;
			double dist = abs(position.z - foePos.z);
			if(dist > range || dist > distance) continue;
			distance = dist;
			collideAgainst = a;
		}
		if(collideAgainst){
			// Now determine how much damage to deal
			switch(collideAgainst->getType()){
				// Soldier object: normal damage
			case a_Soldier:
				collideAgainst->takeDamage(damage);
				break;

				// Tank object: normal damage
			case a_Tank:
				collideAgainst->takeDamage(damage);
				break;

				// Cannon object: normal damage
			case a_Cannon:
				collideAgainst->takeDamage(damage);
				break;

				// Wall object: double damage
			case a_Wall:
				collideAgainst->takeDamage(2 * damage);
				break;

				// Tower object: double damage
			case a_Tower:
				collideAgainst->takeDamage(2 * damage);
				break;
			}
			lastAction = glfwGetTime();
		}
	}
}

void Tank::move(){
	// Check if should move
	double currTime = glfwGetTime();
	if(!isActive) lastTime = currTime;
	Actor *collideAgainst = 0;
	if(shouldMove){
		position.z += ((id > 0)? -1.2f : 1.2f) * (currTime - lastTime);
		lastTime = currTime;
		// Check which opponent (if any) are in this unit's hit range
		for(Actor *a : (id > 0)? foeActors : selfActors){
			if(!a->active()) continue;
			vec3& foePos = a->getPosition();
			if(fabs(position.x - foePos.x) > .25f && a->getType() != a_Tower) continue;
			if(abs(position.z - foePos.z) > range) continue;
			// Found a unit in hit range, stop moving to prepare for attack
			shouldMove = false;
			lastAction = glfwGetTime();
			break;
		}
	}
	else{
		// Check which opponent (if any) are in this unit's hit range
		for(Actor *a : (id > 0)? foeActors : selfActors){
			if(!a->active()) continue;
			vec3& foePos = a->getPosition();
			if(fabs(position.x - foePos.x) > .25f && a->getType() != a_Tower) continue;
			if(abs(position.z - foePos.z) > range) continue;
			// Found a unit in hit range, remain immobile
			collideAgainst = a;
			break;
		}
		// No units found, can move again
		if(!collideAgainst){
			lastTime = glfwGetTime();
			shouldMove = true;
		}
	}
}

void Tank::doAction(){
	Actor *collideAgainst = 0;
	float distance = 10;
	if(!shouldMove){
		// Check which opponent (if any) are in this unit's hit range
		for(Actor *a : (id > 0)? foeActors : selfActors){
			if(!a->active()) continue;
			vec3& foePos = a->getPosition();
			if(fabs(position.x - foePos.x) > .25f && a->getType() != a_Tower) continue;
			double dist = abs(position.z - foePos.z);
			if(dist > range || dist > distance) continue;
			distance = dist;
			collideAgainst = a;
		}
		if(collideAgainst){
			// Now determine how much damage to deal
			switch(collideAgainst->getType()){
				// Soldier object: half damage
			case a_Soldier:
				collideAgainst->takeDamage(damage / 2);
				break;

				// Tank object: double damage
			case a_Tank:
				collideAgainst->takeDamage(damage * 2.0);
				break;

				// Cannon object: third damage
			case a_Cannon:
				collideAgainst->takeDamage(damage / 3.0);
				break;

				// Wall object: normal damage
			case a_Wall:
				collideAgainst->takeDamage(damage);
				break;

				// Tower object: double damage
			case a_Tower:
				collideAgainst->takeDamage(2 * damage);
				break;
			}
		}
		lastAction = glfwGetTime();
	}
}

void Cannon::move(){
	// Check if should move
	double currTime = glfwGetTime();
	if(!isActive) lastTime = currTime;
	Actor *collideAgainst = 0;
	if(shouldMove){
		position.z += ((id > 0)? -.6f : .6f) * (currTime - lastTime);
		lastTime = currTime;
		// Check which opponent (if any) are in this unit's hit range
		for(Actor *a : (id > 0)? foeActors : selfActors){
			if(!a->active()) continue;
			vec3& foePos = a->getPosition();
			if(fabs(position.x - foePos.x) > .25f && a->getType() != a_Tower) continue;
			if(abs(position.z - foePos.z) > range) continue;
			// Found a unit in hit range, stop moving to prepare for attack
			shouldMove = false;
			lastAction = glfwGetTime();
			break;
		}
	}
	else{
		// Check which opponent (if any) are in this unit's hit range
		for(Actor *a : (id > 0)? foeActors : selfActors){
			if(!a->active()) continue;
			vec3& foePos = a->getPosition();
			if(fabs(position.x - foePos.x) > .25f && a->getType() != a_Tower) continue;
			if(abs(position.z - foePos.z) > range) continue;
			// Found a unit in hit range, remain immobile
			collideAgainst = a;
			break;
		}
		// No units found, can move again
		if(!collideAgainst){
			lastTime = glfwGetTime();
			shouldMove = true;
		}
	}
}

void Cannon::doAction(){
	Actor *collideAgainst = 0;
	float distance = 10;
	if(!shouldMove){
		// Check which opponent (if any) are in this unit's hit range
		for(Actor *a : (id > 0)? foeActors : selfActors){
			if(!a->active()) continue;
			vec3& foePos = a->getPosition();
			if(fabs(position.x - foePos.x) > .25f && a->getType() != a_Tower) continue;
			double dist = abs(position.z - foePos.z);
			if(dist > range || dist > distance) continue;
			distance = dist;
			collideAgainst = a;
		}
		if(collideAgainst){
			// Now determine how much damage to deal
			switch(collideAgainst->getType()){
				// Soldier object: quarter damage
			case a_Soldier:
				collideAgainst->takeDamage(damage / 4.0);
				break;

				// Tank object: normal damage
			case a_Tank:
				collideAgainst->takeDamage(damage);
				break;

				// Cannon object: quarter damage
			case a_Cannon:
				collideAgainst->takeDamage(damage / 4.0);
				break;

				// Wall object: almost triple damage
			case a_Wall:
				collideAgainst->takeDamage(2.5 * damage);
				break;

				// Tower object: triple damage
			case a_Tower:
				collideAgainst->takeDamage(3 * damage);
				break;
			}
		}
		lastAction = glfwGetTime();
	}
}