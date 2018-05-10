#ifndef SHAPE_H
#define SHAPE_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "intersection.h"

class Shape {
public:
	Shape(
		const glm::vec3 &sc,
		const bool &refl = false,
		const float &transp = 0.0f,
		const float &refrInd = 0.0f,
		const float &ef = 0.0f,
		const glm::vec3 &lc = glm::vec3(0.0f, 0.0f, 0.0f)) :
		surfaceColor(sc), lightColor(lc),
		refractionIndex(refrInd), reflectivity(refl), transparency(transp), emission(ef)
	{ 
	}

	~Shape() {}

	virtual Intersection intersection(
		const glm::vec3& rayOrig,
		const glm::vec3 &rayDir,
		float epsilon = 0.f) const = 0;

	virtual glm::vec3 getSurfaceColor() const { return surfaceColor; }
	glm::vec3 getLightColor() const { return lightColor; }
	float getRefractionIndex() const { return refractionIndex; }
	float transparencyFactor() const { return transparency; }
	float emissionFactor() const { return emission; }
	bool isLight() const { return (emission > 0.0f); }
	bool refractsLight() const { return refractionIndex != 0; }
	bool reflectsLight() const { return reflectivity; }

private:
	glm::vec3 surfaceColor; 
	glm::vec3 lightColor;
	float emission;
	bool reflectivity;
	float transparency; // in the range [0.0, 1.0]
	float refractionIndex;
};

#endif
