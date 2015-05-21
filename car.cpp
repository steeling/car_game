#include "car.h"
#include "odeBoxBox.h"
#include "GuardRail.h"

#define GLM_FORCE_RADIANS
#define CAR_HEIGHT -1.6f

using namespace std;

Car::Car():
	acceleration(0.01f),
	maxSpeed(1.0f),
	curSpeed(0.0f),
	health(150),
	boostCap(100)
{

}


void Car::load(){
	shape.load("tempCar.obj", "tempCar.mtl");
	pos.y = CAR_HEIGHT;

}

void Car::init(){
	shape.init();
}

bool Car::onFloor(){
	return pos.y <= CAR_HEIGHT + .2f;
}

void Car::draw(MatrixStack &MV, GLint h_pos, GLint h_nor, GLint h_tex, GLint h_MV){
	MV.pushMatrix();
	MV.translate(pos);
	// MV.translate(glm::vec3(0.0f, 0.0f, 0.5f));
	MV.rotate(direction, glm::vec3(0.0f, 1.0f, 0.0f));
	MV.translate(glm::vec3(0.0f, 0.0f, 0.5f));
	MV.rotate(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	MV.translate(glm::vec3(0.0f, 0.0f, -0.5f));
	// MV.translate(glm::vec3(0.0f, 0.0f, -0.5f));	
	glUniformMatrix4fv(h_MV, 1, GL_FALSE, glm::value_ptr(MV.topMatrix()));

	shape.draw(h_pos, h_nor, h_tex);
	MV.popMatrix();
}

void Car::turnLeft(float yaw, float tangent){
	if(onFloor() && health > 0)
		direction = min(0.05f + direction, tangent + atan(1.0f));
}

void Car::turnRight(float yaw, float tangent){
	if(onFloor() && health > 0)
		direction = max(-0.05f + direction, tangent - atan(1.0f));
}

void Car::reset(){
	curSpeed = 0.0f;
	boostCap = 100;
	health = 100;
}

void Car::move(bool boost, bool bumping, bool gas, bool braking){
	if(health == 0) return;
	if(onFloor() && gas){
		curSpeed = min(curSpeed + acceleration / (maxSpeed + 0.00001f - curSpeed), maxSpeed);
		if(bumping){
			curSpeed = max(min(curSpeed - acceleration / (0.00001f + curSpeed), maxSpeed), 0.5f);
			--health;
			pos.y += pos.y > CAR_HEIGHT ? -0.2f : .2f;
		}else
			pos.y = CAR_HEIGHT;
		if(boost && boostCap > 0){
			curSpeed *= 2.0f;
			--boostCap;
		}
	}else if(onFloor()){
		curSpeed = max(curSpeed - curSpeed / (braking ? 5.0f: 20.0f), 0.0f);
		if(curSpeed < 0.05f && braking){
			curSpeed = -.5f;
		}
	}
	velocity.x = -1 * curSpeed * sin(direction);
	velocity.z = -1 * curSpeed * cos(direction);
	velocity.y = 1 * curSpeed * sin(pitch);
	pos += velocity;
	didHitRail();
	wheelsContactRamp();
}

void Car::reallign(float yaw, float tangent){
	direction += yaw > direction ? 0.05f : -0.05f;
	direction = fabs(yaw - direction) < 0.06f ? yaw : direction;
}

void Car::wheelsContactRamp(){
	MatrixStack carMat, rampMat; 
	carMat.pushMatrix();
	carMat.translate(pos);
	carMat.rotate(direction, glm::vec3(0.0f, 1.0f, 0.0f));
	bool madeContact = false;
	for (int i = 0; i < Ramp::ramps.size(); ++i){
		rampMat.pushMatrix();
		rampMat.translate(Ramp::ramps.at(i).position);
		rampMat.rotate(Ramp::ramps.at(i).direction, glm::vec3(0.0f, 1.0f, 0.0f));
		Contacts contact = odeBoxBox(carMat.topMatrix(), glm::vec3(0.5f, 0.5f, 0.5f), rampMat.topMatrix(), Ramp::ramps.at(i).size);
		rampMat.popMatrix();
		if(contact.positions.size() > 0){
			pitch = atan(1.0f);
			madeContact = true;
		}
	}
	carMat.popMatrix();
	if(!madeContact && pos.y > CAR_HEIGHT + .2f){
		pitch -= 0.08f;
	}else if(!madeContact && pos.y < CAR_HEIGHT){
		pos.y = CAR_HEIGHT;
		pitch = pitch * -0.2;
		if(fabs(pitch) <= 0.1f)
			pitch = 0.0f;
	}
}

void straightHit(){

}

void Car::didHitRail(){
	MatrixStack carMat;
	carMat.pushMatrix();
	carMat.translate(pos);
	carMat.rotate(direction, glm::vec3(0.0f, 1.0f, 0.0f));
	Impact impact = GuardRail::didContact(carMat.topMatrix(), glm::vec3(0.5f, 0.5f, 0.5f));		
	carMat.popMatrix();
	if(impact.wall == 1){
		direction -= atan(1.0) / 2;
		health -=10;
	}
	else if(impact.wall == 2){
		direction += atan(1.0) / 2;
		health -=10;
	}
}