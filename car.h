//
// Sean Teeling
// November, 2014
//

#pragma  once
#ifndef __CAR__
#define __CAR__

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "MatrixStack.h"
#include "Shape.h"
#include "Ramp.h"

class MatrixStack;

class Car
{
public:
	
	Car();
	void init();
	void load();
	void draw(MatrixStack &MV, GLint h_pos, GLint h_nor, GLint h_tex, GLint h_MV);
	void turnLeft(float yaw, float tangent);
	void turnRight(float yaw, float tangent);
	void reallign(float yaw, float tangent);
	void wheelsContactRamp();
	void move(bool boost, bool bumping, bool gas, bool braking);
	void reset();
	bool onFloor();

	glm::vec3 pos;
	glm::vec3 velocity;
	float maxSpeed;
	float curSpeed;
	float direction;
	float pitch;
	float acceleration;	
	int health;
	int boostCap;


private:
	void didHitRail();
	Shape shape;
	std::vector<Ramp> ramps;
};

#endif
