#ifndef HPLANE_H
#define HPLANE_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "intersection.h"
#include "shape.h"

class HPlane : public Shape {
public:
	HPlane(
		float height,
		glm::vec2 size,
		const glm::vec3 &sc1,
		const glm::vec3 &sc2,
		const bool &refl = false,
		const float &transp = 0.0f,
		const float &refrInd = 0.0f) :
		Shape(sc1, refl, transp, refrInd),
		height(height), size(size), surfaceColor1(sc1), surfaceColor2(sc2)
	{ 
	}

	~HPlane() {}

	float getHeight() const { return height; }
	glm::vec2 getSize() const { return size; }
	bool isHigher(glm::vec3 p) const { return (p.y > height); }

	Intersection intersection(
		const glm::vec3& rayOrig,
		const glm::vec3 &rayDir,
		float epsilon) const 
	{
		Intersection ret;

		glm::vec3 origin = rayOrig;
		if (epsilon > 0.f)
		{
			origin += glm::normalize(rayDir) * epsilon;
		}

		glm::vec3 hitPos;
		if (intersect(origin, rayDir, hitPos))
		{
			ret.intersected = true;
			ret.distHit = glm::distance(rayOrig, hitPos);
			ret.posHit = hitPos;
			ret.normalHit = (isHigher(rayOrig) ? glm::vec3(0, 1, 0) : glm::vec3(0, -1, 0));
			ret.object = (Shape*)this;
			ret.isInside = false;
			ret.colorHit = getSurfaceColor(ret.posHit);
		}
		return ret;
	}
	glm::vec3 getSurfaceColor(glm::vec3 coord) const
	{
		if ((int)(floorf(abs(coord.x) / 10.f) + floorf(abs(coord.y) / 10.f)) % 2 == 0)
		{
			return surfaceColor1;
		}
		return surfaceColor2;
	}

private:
	bool intersect(const glm::vec3 &rayorig, const glm::vec3 &raydir, glm::vec3 &hitPos) const
	{
		if (raydir.y == 0
			|| (rayorig.y > height && raydir.y > 0)
			|| (rayorig.y < height && raydir.y < 0))
		{
			return false;
		}
		const float d = (height - rayorig.y) / raydir.y;
		const glm::vec3 pos = rayorig + raydir * d;
		if (abs(pos.x) < size.x && abs(pos.y) < size.y)
		{
			hitPos = pos;
			return true;
		}
		return false;
	}

	glm::vec2 size;
	float height; 
	glm::vec3 surfaceColor1; 
	glm::vec3 surfaceColor2;
};

#endif
