#ifndef CUSTOM_MATH_H
#define CUSTOM_MATH_H

#include "Primitives.h"

#define CMIN(a, b) (((a) < (b)) ? (a) : (b))
#define CMAX(a, b) (((a) > (b)) ? (a) : (b))
#define M_PI 3.14159265359f
constexpr float EPSILON = 0.001f;

namespace CMATH
{
	float DistanceSquared(Point3D const& p1, Point3D const& p2);
	float DistanceSquared(glm::vec3 const& p1, glm::vec3 const& p2);
	float DistanceSquared(float a, float b);
	float Distance(Point3D const& p1, Point3D const& p2);
	float Distance(glm::vec3 const& p1, glm::vec3 const& p2);
	float Clamp(float val, float min, float max);
}

#endif // !CUSTOM_MATH_H
