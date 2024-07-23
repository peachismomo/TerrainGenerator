#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <glm/glm.hpp>

#define COPLANAR 0
#define INSIDE 1
#define OUTSIDE -1

struct Point3D
{
	Point3D(glm::vec3 np) { p = np; }
	Point3D() { p = glm::vec3(0.f); }
	glm::vec3 p{};
};

struct Plane
{
	glm::vec4 normal{ 0.f, 1.f, 0.f, 0.f };
};

struct Triangle
{
	Point3D p1{ glm::vec3(-0.2f, -0.1f, 0.f) }, p2{ glm::vec3(0.2f, -0.1f, 0.f) }, p3{ glm::vec3(0.f, 0.2f, 0.f) };
};

struct BoundingSphere
{
	BoundingSphere() { position = Point3D(); radius = 0.f; }
	BoundingSphere(Point3D const& pos, float r) { position = pos; radius = r; }
	Point3D position{};
	float radius{ 0.5f };
};

struct AABB
{
	Point3D center{};
	Point3D half_extent{ glm::vec3(0.5f) };
};

struct OBB : public AABB
{
	glm::mat4 m_rot_mtx;
};

struct BEllipse : public BoundingSphere
{
	glm::mat4 m_rot_mtx;
	glm::vec3 m_half_axes;
	virtual ~BEllipse() = default;
};

struct Ray
{
	Point3D origin{};
	Point3D direction{ glm::vec3(0.f, 1.f, 0.f) };
};

namespace TESTS
{
	/*
	* BASIC INTERSECTION
	*/
	bool SphereSphere(BoundingSphere const& sphere1, BoundingSphere const& sphere2);

	bool AABBSphere(AABB const& aabb, BoundingSphere const& sphere);

	bool SphereAABB(BoundingSphere const& sphere, AABB const& aabb);

	bool AABBAABB(AABB const& aabb1, AABB const& aabb2);

	/*
	* POINT-BASED INTERSECTION
	*/
	bool PointSphere(Point3D point, BoundingSphere const& sphere);

	bool PointAABB(Point3D point, AABB const& aabb);

	int PointPlane(Point3D point, Plane plane, float epsilon = 0.01f);

	bool PointTriangle(Point3D point, Triangle const& triangle);

	/*
	* RAY-BASED INTERSECTION
	*/
	bool RayPlane(Ray const& ray, Plane plane, float epsilon = 0.01f);

	bool RayTriangle(Ray const& ray, Triangle const& triangle);

	bool RayAABB(Ray const& ray, AABB const& aabb);

	bool RaySphere(Ray const& ray, BoundingSphere const& sphere);

	/*
	* PLANE-BASED INTERSECTION
	*/
	int PlaneAABB(Plane plane, AABB const& aabb);

	int PlaneSphere(Plane plane, BoundingSphere const& sphere);

	/*
	* HELPERS
	*/
	float TriangleArea(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);
	glm::vec3 BarycentricCoords(const glm::vec3& point, const Triangle& triangle);
}

#endif // !PRIMITIVES_H
