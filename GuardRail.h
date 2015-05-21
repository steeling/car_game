//
// Sean Teeling
// November, 2014
//

#pragma  once
#ifndef __GUARDRAIL__
#define __GUARDRAIL__

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "MatrixStack.h"
#include "Shape.h"
#include "odeBoxBox.h"

struct Impact
{
	// Contact points in world space
	glm::vec3 normal;
	// Contact normal (same for all points)
	int wall;
} typedef Impact;

using namespace std;

class MatrixStack;

class GuardRail
{
public:
	
	GuardRail(glm::vec3 pos, float radius, float dir, bool inner);
	static void init();
	static void load();
	static void populate(float trackRadius, float trackWidth);

	void draw(MatrixStack &MV, GLint h_pos, GLint h_nor, GLint h_tex, GLint h_MV);

	static Impact didContact(glm::mat4 carMat, glm::vec3 carSize);

	glm::vec3 position;
	glm::vec3 size;
	float direction;
	bool isInner;

	static vector<GuardRail> rails;
	
private:
	static Shape shape;
};

#endif
