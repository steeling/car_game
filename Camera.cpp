//
// sueda
// November, 2014
//

#include "Camera.h"
#include "MatrixStack.h"
#include <iostream>
#define GLM_FORCE_RADIANS
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/transform.hpp"

using namespace std;
#define FLOOR_SIZE 150.0f
#define CAM_HEIGHT -.3f


Camera::Camera() :
	aspect(1.0f),
	fovy(30.0f/180.0f*M_PI),
	znear(0.1f),
	zfar(1000.0f),
	position(0.0f, CAM_HEIGHT, 0.0f),
	pitch(-5.0f * 3.14159/90) ,
	yaw(0.0f)
{
}

Camera::~Camera()
{
	
}

void Camera::loadCar(){
	car.load();
}

void Camera::initCar(){
	car.init();

}

void Camera::drawCar(MatrixStack &MV, GLint h_pos, GLint h_nor, GLint h_tex, GLint h_MV){
	//car.pos = position + glm::vec3(-5.5f * sin(yaw) , -1.4f, -5.5f * cos(yaw));;
	
	car.draw(MV, h_pos, h_nor, h_tex, h_MV);
}

void Camera::setRadius(float rad, float tWidth){
	trackRadius = rad;
	trackWidth = tWidth;
	car.pos.x = rad + trackWidth / 2;
}

void Camera::setWindowSize(float w, float h)
{
	aspect = w/h;
	width = w;
	height = h;
}

bool Camera::moving(){
	return car.curSpeed < 0.01f || car.curSpeed > 0.05f;
}
void Camera::update(const bool *keys, const glm::vec2 &mouse)
{
	// if(mouse.x > 0.0f && mouse.x < width && mouse.y > 0.0f && mouse.y < height){
	// 	float dx = mouse.x - width / 2;
	// 	float dy = mouse.y - height / 2;
	// 	yaw = -4 * dx * glm::atan(1.0f)*4 / width;
	// 	pitch = -1 * dy * glm::atan(1.0f)*4 / height;
	// }

	// Update camera position and orientation here
	if(keys['r']){
		car.reset();
	}
	else if(moving() && keys['a']){
		car.turnLeft(yaw, tangent);
		yaw = yaw < tangent + atan(1) / 2? yaw + .03f : yaw;
		yaw = min(0.05f + yaw, tangent + atan(1.0f) / 2);
		// position += glm::vec3(0.5f * sin(yaw - glm::atan(1.0f) * 2) , 0.0f, 0.5f * cos(yaw - glm::atan(1.0f) * 2));
	}else if(moving() && keys['d']){
		car.turnRight(yaw, tangent);
		yaw = yaw > tangent ? yaw - .03f : yaw;
		yaw = max(-0.03f + yaw, tangent - atan(1.0f) / 2);
		// position += glm::vec3(-0.5f * sin(yaw - glm::atan(1.0f) * 2) , 0.0f, -0.5f * cos(yaw - glm::atan(1.0f) * 2));
	} else {
		yaw += yaw < tangent + 0.1f ? 0.01f : 0.0f;
		if(moving())
			car.reallign(yaw, tangent);
	}

	float pi = glm::atan(1.0f)*4;

	int xSign = position.x < 0 ? -1 : 1;
	int zSign = position.z < 0 ? -1 : 1;


	float distanceFromCenter = glm::sqrt(car.pos.x * car.pos.x + car.pos.z * car.pos.z);
	distanceFromCenter += car.pos.z > 0 ? 3.0f : -3.0f; 

	if(xSign == -1 && zSign == -1){
		tangent = pi - atan(position.z / position.x);
	}else if(xSign == 1 && zSign == -1){
		tangent = -1 * atan(position.z / position.x);
	}else if(xSign == -1 && zSign == 1){
		tangent = pi - atan(position.z / position.x);
	}else if(xSign == 1 && zSign == 1){
		tangent = 2 * pi - atan(position.z / position.x);
	}
	bool bumping;
	if((distanceFromCenter < trackRadius || distanceFromCenter > trackRadius + trackWidth)){
		bumping = true;
	}else{
		bumping = false;
	}

	car.move(keys[' '], bumping, keys['w'], keys['s']);
}


void Camera::applyProjectionMatrix(MatrixStack *P) const
{
	P->perspective(fovy, aspect, znear, zfar);
}

void Camera::applyViewMatrix(MatrixStack *MV) const
{
	// Create the translation and rotation matrices

	glm::mat4 T = glm::translate(car.pos);
	glm::mat4 X = glm::translate(glm::vec3(5.5f * sin(yaw) , 1.4f, 5.5f * cos(yaw)));

	// The matrix C is the product of these matrices
	glm::mat4 C = X * T * glm::rotate(yaw, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(pitch, glm::vec3(1.0f, 0.0f, 0.0f)); // Also apply rotations here
	// The view matrix is the inverse
	glm::mat4 V = glm::inverse(C);
	// Add to the matrix stack
	MV->multMatrix(V);

//draw HUD
	glLineWidth(10); 
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(-0.9, 0.9, -.5);
	glVertex3f(-0.9 + 0.8 *  car.health / 150.0, 0.9, -.5);
	glEnd();


	glLineWidth(10); 
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.1, 0.9, -.5);
	glVertex3f(0.1 + 0.8 * car.boostCap / 100.0, 0.9, -.5);
	glEnd();

	glEnd();
}
