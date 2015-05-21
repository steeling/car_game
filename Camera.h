//
// sueda
// November, 2014
//

#pragma  once
#ifndef __Camera__
#define __Camera__

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "GLSL.h"
#include "car.h"
#include "Shape.h"
#include "Ramp.h"



class MatrixStack;

class Camera
{
public:
	
	Camera();
	virtual ~Camera();
	void setWindowSize(float w, float h);
	void update(const bool *keys, const glm::vec2 &mouse);
	void applyProjectionMatrix(MatrixStack *P) const;
	void applyViewMatrix(MatrixStack *MV) const;
	void setRadius(float rad, float tWidth);
	void loadCar();
	void initCar();
	void drawCar(MatrixStack &MV, GLint h_pos, GLint h_nor, GLint h_tex, GLint h_MV);
	glm::vec3 position;

	
private:

	bool moving();
	float aspect;
	float fovy;
	float znear;
	float zfar;
	float pitch;
	float yaw;
	float width;
	float height;
	float trackRadius;
	float trackWidth;
	float tangent;
	Car car;
	Shape shape;
	glm::vec2 mousePrev;

};

#endif
