#ifndef _ODEBOXBOX_
#define _ODEBOXBOX_

#include <vector>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"

struct Contacts
{
	// Contact points in world space
	std::vector<glm::vec3> positions;
	// Contact normal (same for all points)
	glm::vec3 normal;
};

Contacts odeBoxBox(glm::mat4 M1, glm::vec3 dimensions1, glm::mat4 M2, glm::vec3 dimensions2);

#endif
