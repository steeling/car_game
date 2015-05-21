#include "GuardRail.h"

#define GLM_FORCE_RADIANS


Shape GuardRail::shape;
vector<GuardRail> GuardRail::rails;


GuardRail::GuardRail(glm::vec3 pos, float radius, float dir, bool inner)
{
	position = pos;
	if(inner)
		size = glm::vec3(0.5f, 0.5f, 2 * radius * sin(atan(1.0f) / 2));
	else
		size = glm::vec3(0.5f, 0.5f,  radius * cos(atan(1.0f) / 2));
	direction = dir;
	isInner = inner;
}

void GuardRail::load(){
	shape.load("cubeT.obj", "tempCar.mtl");
}

void GuardRail::init(){
	shape.init();
}

void GuardRail::populate(float trackRadius, float trackWidth){

	int numSections = 8;


	float r, theta, distance;
	for(float theta = 0.0f; theta < atan(1.0f) * 8; theta += atan(1.0f)){
		rails.push_back(GuardRail( glm::vec3((trackRadius - (trackWidth / 2)) * cos(theta), -1.5f, -2 + (trackRadius - trackWidth / 2) * sin(theta) * -1), trackRadius, theta, true));
		rails.push_back(GuardRail( glm::vec3((trackRadius + 2 * trackWidth) * cos(theta), -1.5f, (trackRadius + 2 *trackWidth) * sin(theta) * -1), trackRadius + 2 * trackWidth, theta, false));
	}
}

void GuardRail::draw(MatrixStack &MV, GLint h_pos, GLint h_nor, GLint h_tex, GLint h_MV){
	float pi = atan(1.0) * 4;

	MV.pushMatrix();
	MV.translate(position);
	MV.rotate(direction, glm::vec3(0.0f, 1.0f, 0.0f));
	MV.translate(glm::vec3(0.0f, 0.0f, size.z / 2));
	MV.scale(size);
	MV.translate(glm::vec3(0.0f, 0.0f, -0.5f));
	glUniformMatrix4fv(h_MV, 1, GL_FALSE, glm::value_ptr(MV.topMatrix()));

	shape.draw(h_pos, h_nor, h_tex);
	MV.popMatrix();
}

Impact GuardRail::didContact(glm::mat4 carMat, glm::vec3 carSize){
	MatrixStack railMat; 
	Impact impact;
	impact.wall = 0;
	for(int i = 0; i < rails.size(); ++i){
		railMat.pushMatrix();
		railMat.translate(rails.at(i).position);
		railMat.rotate(rails.at(i).direction, glm::vec3(0.0f, 1.0f, 0.0f));

		Contacts contact = odeBoxBox(carMat, glm::vec3(0.5f, 0.5f, 0.5f), railMat.topMatrix(), rails.at(i).size);
		if(contact.positions.size() > 0){
			impact.wall = rails.at(i).isInner ? 1 : 2;
			impact.normal = contact.normal;
			break;
		}
		railMat.popMatrix();
	}
	return impact;
}

