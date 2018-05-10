#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Shape;

struct Intersection
{
	bool intersected = false;
	float distHit = FLT_MAX;
	glm::vec3 posHit = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 normalHit = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 colorHit = glm::vec3(0.f, 0.f, 0.f);
	bool isInside = false;
	Shape* object = nullptr;
};
#endif
