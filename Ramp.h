//
// Sean Teeling
// November, 2014
//

#pragma  once
#ifndef __RAMP__
#define __RAMP__

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "MatrixStack.h"
#include "Shape.h"

using namespace std;

class MatrixStack;

class Ramp
{
public:
	
	Ramp(glm::vec3 pos, float dir, glm::vec3 dims);
	static void init();
	static void load();
	static void populate(int numRamps, float trackRadius, float trackWidth);

	void draw(MatrixStack &MV, GLint h_pos, GLint h_nor, GLint h_tex, GLint h_MV);

	glm::vec3 position;
	glm::vec3 size;
	float direction;

	static vector<Ramp> ramps;
	
private:
	static Shape shape;
};

#endif
