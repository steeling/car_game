#include "ramp.h"
#include "odeBoxBox.h"

#define GLM_FORCE_RADIANS


Shape Ramp::shape;
vector<Ramp> Ramp::ramps;


Ramp::Ramp(glm::vec3 pos, float dir, glm::vec3 dims)
{
	position = pos;
	direction = dir;
	size = dims;
}

void Ramp::load(){
	shape.load("ramp.obj", "tempCar.mtl");
}

void Ramp::init(){
	shape.init();
}

void Ramp::populate(int numRamps, float trackRadius, float trackWidth){
	float r, theta, distance;
	MatrixStack rampMat1, rampMat2;
	int temp;
	for(int i = 0; i < numRamps; ++i){
		r = (float) rand() / RAND_MAX;
		theta = r * atan(1.0f) * 8;
		r = (float) rand() / RAND_MAX;

		distance = 110.0f;
		rampMat2.pushMatrix();
		rampMat2.translate(glm::vec3(distance * cos(theta), -1.0f, -1 * distance * sin(theta)));
		rampMat2.rotate(theta, glm::vec3(0.0f, 1.0f, 0.0f));

		temp = i;
		for (int j = 0; j < ramps.size(); ++j){
			rampMat1.pushMatrix();
			rampMat1.translate(ramps.at(j).position);
			rampMat1.rotate(ramps.at(j).direction, glm::vec3(0.0f, 1.0f, 0.0f));


			if(odeBoxBox(rampMat2.topMatrix(), glm::vec3(2.0f, 2.0f, 2.0f), rampMat1.topMatrix(), ramps.at(j).size).positions.size() > 0){
				--i;
				rampMat1.popMatrix();
				break;
			}
		}
		rampMat2.popMatrix();
		if(temp == i)
			ramps.push_back(Ramp(glm::vec3(distance * cos(theta), -1.0f, -1 * distance * sin(theta)), theta, glm::vec3(2.0f, 2.0f, 2.0f)));
	}
}

void Ramp::draw(MatrixStack &MV, GLint h_pos, GLint h_nor, GLint h_tex, GLint h_MV){
	float pi = atan(1.0) * 4;

	MV.pushMatrix();
	MV.translate(position);
	MV.rotate(direction, glm::vec3(0.0f, 1.0f, 0.0f));
	MV.scale(size);
	glUniformMatrix4fv(h_MV, 1, GL_FALSE, glm::value_ptr(MV.topMatrix()));


	shape.draw(h_pos, h_nor, h_tex);
	MV.popMatrix();
}

