#ifndef SPHERE_H
#define SPHERE_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shape.h"

class Sphere : public Shape {
public:
	Sphere(
		const glm::vec3 &c,
		const float &r,
		const glm::vec3 &sc,
		const bool &refl = false,
		const float &transp = 0.0f,
		const float &refrInd = 0.0f,
		const float &ef = 0.0f,
		const glm::vec3 &lc = glm::vec3(0.0f, 0.0f, 0.0f)) :
		Shape(sc, refl, transp, refrInd, ef, lc),
		center(c), radius(r), radius2(r * r)
	{}

	~Sphere() {}

	glm::vec3 getCenter() const { return center; }

	Intersection intersection(const glm::vec3& rayOrig, const glm::vec3 &rayDir, float epsilon = 0.f) const
	{

		float inter0 = INFINITY;
		float inter1 = INFINITY;

		Intersection ret;

		glm::vec3 origin = rayOrig;
		if (epsilon > 0.f)
		{
			origin += glm::normalize(rayDir) * epsilon;
		}

		if (intersect(origin, rayDir, inter0, inter1))
		{
			if (inter0 < 0) {
				inter0 = inter1;
			}

			ret.intersected = true;
			ret.distHit = inter0 + epsilon;
			ret.posHit = rayOrig + rayDir * ret.distHit;
			ret.normalHit = ret.posHit - getCenter();
			ret.normalHit = glm::normalize(ret.normalHit);
			ret.object = (Shape*)this;

			// If the normal and the view direction are not opposite to each other
			// reverse the normal direction. That also means we are inside the sphere so set
			// the inside bool to true.
			float dotProd = glm::dot(rayDir, ret.normalHit);
			ret.isInside = false;
			if (dotProd > 0) {
				ret.normalHit = -ret.normalHit;
				ret.isInside = true;
			}

			ret.colorHit = getSurfaceColor();
		}
		return ret;
	}

private:
	bool intersect(const glm::vec3 &rayorig, const glm::vec3 &raydir, float &t0, float &t1) const
	{
		glm::vec3 l = center - rayorig;
		float tca = glm::dot(l, raydir);
		if (tca < 0) return false;
		float d2 = glm::dot(l, l) - tca * tca;
		if (d2 > radius2) return false;
		float thc = sqrt(radius2 - d2);
		t0 = tca - thc;
		t1 = tca + thc;

		return true;
	}

	glm::vec3 center; 
	float radius, radius2; 
};

#endif
