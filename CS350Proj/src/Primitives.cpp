#include "Primitives.h"

#include "CustomMath.h"

bool TESTS::SphereSphere(BoundingSphere const& sphere1, BoundingSphere const& sphere2)
{
	float dist_sq{ CMATH::DistanceSquared(sphere1.position, sphere2.position) };
	if (dist_sq <= powf((sphere1.radius + sphere2.radius), 2))
		return true;
	return false;
}

bool TESTS::AABBSphere(AABB const& aabb, BoundingSphere const& sphere)
{
	float closestX = CMAX(aabb.center.p.x - aabb.half_extent.p.x,
		CMIN(sphere.position.p.x, aabb.center.p.x + aabb.half_extent.p.x));
	float closestY = CMAX(aabb.center.p.y - aabb.half_extent.p.y,
		CMIN(sphere.position.p.y, aabb.center.p.y + aabb.half_extent.p.y));
	float closestZ = CMAX(aabb.center.p.z - aabb.half_extent.p.z,
		CMIN(sphere.position.p.z, aabb.center.p.z + aabb.half_extent.p.z));

	// Calculate the squared distance from the closest point to the sphere's center
	Point3D closestPoint(glm::vec3(closestX, closestY, closestZ));
	float distanceSquared = CMATH::DistanceSquared(closestPoint, sphere.position);

	// Check if the squared distance is less than or equal to the squared radius of the sphere
	return distanceSquared <= (sphere.radius * sphere.radius);
}

bool TESTS::SphereAABB(BoundingSphere const& sphere, AABB const& aabb)
{
	return AABBSphere(aabb, sphere);
}

bool TESTS::AABBAABB(AABB const& aabb1, AABB const& aabb2)
{
	Point3D aabb1_min = aabb1.center.p - aabb1.half_extent.p;
	Point3D aabb1_max = aabb1.center.p + aabb1.half_extent.p;

	Point3D aabb2_min = aabb2.center.p - aabb2.half_extent.p;
	Point3D aabb2_max = aabb2.center.p + aabb2.half_extent.p;

	if (aabb1_min.p.x > aabb2_max.p.x ||
		aabb2_min.p.x > aabb1_max.p.x ||
		aabb1_max.p.y < aabb2_min.p.y ||
		aabb2_max.p.y < aabb1_min.p.y)
		return false;

	return true;
}

bool TESTS::PointSphere(Point3D point, BoundingSphere const& sphere)
{
	if (CMATH::DistanceSquared(point, sphere.position) <= powf(sphere.radius, 2))
		return true;
	return false;
}

bool TESTS::PointAABB(Point3D point, AABB const& aabb)
{
	Point3D aabb_min = aabb.center.p - aabb.half_extent.p;
	Point3D aabb_max = aabb.center.p + aabb.half_extent.p;

	if (point.p.x <= aabb_max.p.x && point.p.y <= aabb_max.p.y &&
		point.p.x >= aabb_min.p.x && point.p.y >= aabb_min.p.y)
		return true;

	return false;
}

int TESTS::PointPlane(Point3D point, Plane plane, float epsilon)
{
	glm::vec4 p = glm::vec4(point.p.x, point.p.y, point.p.z, -1.f);
	float w = glm::dot(plane.normal, p);
	
	if (w > epsilon)
		return INSIDE;
	else if (w < -epsilon)
		return OUTSIDE;
	else 
		return COPLANAR;
}

float TESTS::TriangleArea(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) 
{
	glm::vec3 edge1 = v1 - v0;
	glm::vec3 edge2 = v2 - v0;
	return 0.5f * glm::length(glm::cross(edge1, edge2));
}

glm::vec3 TESTS::BarycentricCoords(const glm::vec3& point, const Triangle& triangle)
{
	float totalArea = TriangleArea(triangle.p1.p, triangle.p2.p, triangle.p3.p);
	float s1 = TriangleArea(triangle.p1.p, point, triangle.p2.p);
	float s2 = TriangleArea(triangle.p2.p, point, triangle.p3.p);
	float s3 = TriangleArea(triangle.p3.p, point, triangle.p1.p);

	float a = s1 / totalArea;
	float b = s2 / totalArea;
	float c = s3 / totalArea;

	return glm::vec3(a, b, c);
}

bool TESTS::PointTriangle(Point3D point, Triangle const& triangle)
{
	glm::vec3 b_coords = BarycentricCoords(point.p, triangle);
	if (!(b_coords.x >= 0 &&
		b_coords.y >= 0 &&
		b_coords.z >= 0 &&
		b_coords.x <= 1 &&
		b_coords.y <= 1 &&
		b_coords.z <= 1))
		return false;

	glm::vec3 U = triangle.p2.p - triangle.p1.p;
	glm::vec3 V = triangle.p3.p - triangle.p1.p;

	glm::vec3 N = glm::cross(U, V);
	glm::vec3 n = glm::normalize(N);

	float w = -glm::dot(n, triangle.p1.p);
	Plane plane;
	plane.normal = glm::vec4(n, w);

	return PointPlane(point, plane, 0.01f) == COPLANAR;
}

bool TESTS::RayPlane(Ray const& ray, Plane plane, float epsilon)
{
	glm::vec3 normal = glm::vec3(plane.normal.x, plane.normal.y, plane.normal.z);
	float denom = glm::dot(normal, ray.direction.p);

	if (denom > -epsilon && denom < epsilon)
		return false;

	float num = plane.normal.w - glm::dot(normal, ray.origin.p);
	float t = num / denom;
	if (t < 0)
		return false;
	return true;
}

bool TESTS::RayTriangle(Ray const& ray, Triangle const& triangle)
{
	glm::vec3 v0v1 = triangle.p2.p - triangle.p1.p;
	glm::vec3 v0v2 = triangle.p3.p - triangle.p1.p;
	glm::vec3 pvec = glm::cross(ray.direction.p, v0v2);
	float det = glm::dot(v0v1, pvec);

	if (glm::abs(det) < EPSILON)
		return false;

	float invDet = 1 / det;
	glm::vec3 tvec = ray.origin.p - triangle.p1.p;
	float u = glm::dot(tvec, pvec) * invDet;
	if (u < 0 || u > 1)
		return false;

	glm::vec3 qvec = glm::cross(tvec, v0v1);
	float v = glm::dot(ray.direction.p, qvec) * invDet;
	if (v < 0 || u + v > 1) return false;

	float t = glm::dot(v0v2, qvec) * invDet;
	return t > 0;
}

bool TESTS::RayAABB(Ray const& ray, AABB const& aabb)
{
	glm::vec3 invDir = 1.0f / ray.direction.p;
	glm::vec3 tMin = (aabb.center.p - aabb.half_extent.p - ray.origin.p) * invDir;
	glm::vec3 tMax = (aabb.center.p + aabb.half_extent.p - ray.origin.p) * invDir;

	glm::vec3 t1 = glm::min(tMin, tMax);
	glm::vec3 t2 = glm::max(tMin, tMax);

	float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
	float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);

	return tNear <= tFar && tFar > 0;
}

bool TESTS::RaySphere(Ray const& ray, BoundingSphere const& sphere)
{
	glm::vec3 oc = ray.origin.p - sphere.position.p;
	float a = glm::dot(ray.direction.p, ray.direction.p);
	float b = 2.0f * glm::dot(oc, ray.direction.p);
	float c = glm::dot(oc, oc) - sphere.radius * sphere.radius;
	float discriminant = b * b - 4 * a * c;
	return (discriminant > 0);
}

int TESTS::PlaneAABB(Plane plane, AABB const& aabb)
{
	glm::vec3 c = aabb.center.p - aabb.half_extent.p;

	glm::vec3 aabb_min = aabb.center.p - aabb.half_extent.p;
	glm::vec3 aabb_max = aabb.center.p + aabb.half_extent.p;

	float r = 0;
	for (int i = 0; i < 3; ++i) {
		float e = (aabb_max[i] - aabb_min[i]) * 0.5f;
		r += e * glm::abs(plane.normal[i]);
		c[i] += e;
	}

	BoundingSphere sphere;
	sphere.position = c;
	sphere.radius = r;
	return TESTS::PlaneSphere(plane, sphere);
}

int TESTS::PlaneSphere(Plane plane, BoundingSphere const& sphere)
{
	return PointPlane(sphere.position, plane, sphere.radius);
}