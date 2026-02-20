#include "CustomMath.h"

float CMATH::DistanceSquared(Point3D const& p1, Point3D const& p2)
{
	return powf((p2.p.x - p1.p.x), 2.f) + powf((p2.p.y - p1.p.y), 2.f) + powf((p2.p.z - p1.p.z), 2.f);
}

float CMATH::DistanceSquared(glm::vec3 const& p1, glm::vec3 const& p2)
{
	return DistanceSquared(Point3D(p1), Point3D(p2));
}

float CMATH::DistanceSquared(float a, float b)
{
	return (a - b) * (a - b);
}

float CMATH::Distance(Point3D const& p1, Point3D const& p2)
{
	return sqrtf(DistanceSquared(p1, p2));
}

float CMATH::Distance(glm::vec3 const& p1, glm::vec3 const& p2)
{
	return Distance(Point3D(p1), Point3D(p2));
}

float CMATH::Clamp(float val, float min_v, float max_v)
{
	return CMAX(min_v, CMIN(val, max_v));
}
