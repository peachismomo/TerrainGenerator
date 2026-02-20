#include "Primitives.h"

#include "CustomMath.h"
#include <limits>
#include <glm/gtx/norm.hpp>

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

bool TESTS::TriContainsVertex(const Triangle2D& triangle, glm::vec2 const& point)
{
	return TESTS::EqualEpsilon(triangle.p1, point) ||
		TESTS::EqualEpsilon(triangle.p2, point) ||
		TESTS::EqualEpsilon(triangle.p3, point);
}

bool TESTS::CircumCircleContains(const Triangle2D& triangle, glm::vec2 const& point)
{
	const double ab = triangle.p1.x * triangle.p1.x + triangle.p1.y * triangle.p1.y;
	const double cd = triangle.p2.x * triangle.p2.x + triangle.p2.y * triangle.p2.y;
	const double ef = triangle.p3.x * triangle.p3.x + triangle.p3.y * triangle.p3.y;

	const double ax = triangle.p1.x;
	const double ay = triangle.p1.y;
	const double bx = triangle.p2.x;
	const double by = triangle.p2.y;
	const double cx = triangle.p3.x;
	const double cy = triangle.p3.y;

	const double circum_x = (ab * (cy - by) + cd * (ay - cy) + ef * (by - ay)) / (ax * (cy - by) + bx * (ay - cy) + cx * (by - ay));
	const double circum_y = (ab * (cx - bx) + cd * (ax - cx) + ef * (bx - ax)) / (ay * (cx - bx) + by * (ax - cx) + cy * (bx - ax));

	const glm::vec2 circum(circum_x / 2, circum_y / 2);
	const float circum_radius = glm::distance2(triangle.p1, circum);
	const float dist = glm::distance2(point, circum);
	return dist <= circum_radius;
}

bool TESTS::EqualTriangles(const Triangle2D& t1, const Triangle2D& t2)
{
	return	(EqualEpsilon(t1.p1, t2.p1) || EqualEpsilon(t1.p1, t2.p2) || EqualEpsilon(t1.p1, t2.p3)) &&
		(EqualEpsilon(t1.p2, t2.p1) || EqualEpsilon(t1.p2, t2.p2) || EqualEpsilon(t1.p2, t2.p3)) &&
		(EqualEpsilon(t1.p3, t2.p1) || EqualEpsilon(t1.p3, t2.p2) || EqualEpsilon(t1.p3, t2.p3));
}

bool TESTS::EqualEdges(const Edge2D& e1, const Edge2D& e2)
{
	return	(EqualEpsilon(e1.p1, e2.p1) && EqualEpsilon(e1.p2, e2.p2)) ||
		(EqualEpsilon(e1.p1, e2.p2) && EqualEpsilon(e1.p2, e2.p1));
}

bool TESTS::EqualEpsilon(float x, float y, int ulp)
{
	return fabsf(x - y) <= std::numeric_limits<float>::epsilon() * fabsf(x + y) * static_cast<float>(ulp) || fabsf(x - y) < std::numeric_limits<float>::min();
}

bool TESTS::EqualEpsilon(glm::vec2 const& a, glm::vec2 const& b)
{
	return EqualEpsilon(a.x, b.x) && EqualEpsilon(a.y, b.y);
}

std::vector<Triangle2D> TESTS::Triangulate(std::vector<glm::vec2>& vertices)
{
	std::vector<Triangle2D> _triangles{};
	float minX = vertices[0].x;
	float minY = vertices[0].y;
	float maxX = minX;
	float maxY = minY;

	for (std::size_t i = 0; i < vertices.size(); ++i)
	{
		if (vertices[i].x < minX) minX = vertices[i].x;
		if (vertices[i].y < minY) minY = vertices[i].y;
		if (vertices[i].x > maxX) maxX = vertices[i].x;
		if (vertices[i].y > maxY) maxY = vertices[i].y;
	}

	const float dx = maxX - minX;
	const float dy = maxY - minY;
	const float deltaMax = CMAX(dx, dy);
	const float midx = (minX + maxX) / 2;
	const float midy = (minY + maxY) / 2;

	const glm::vec2 p1(midx - 20 * deltaMax, midy - deltaMax);
	const glm::vec2 p2(midx, midy + 20 * deltaMax);
	const glm::vec2 p3(midx + 20 * deltaMax, midy - deltaMax);

	// Create a list of triangles, and add the supertriangle in it
	_triangles.push_back(Triangle2D(p1, p2, p3));

	for (auto p = begin(vertices); p != end(vertices); p++)
	{
		std::vector<Edge2D> polygon;

		for (auto& t : _triangles)
		{
			if (CircumCircleContains(t, *p))
			{
				t.valid = true;
				polygon.push_back(Edge2D{ t.p1, t.p2 });
				polygon.push_back(Edge2D{ t.p2, t.p3 });
				polygon.push_back(Edge2D{ t.p3, t.p1 });
			}
		}

		_triangles.erase(std::remove_if(begin(_triangles), end(_triangles), [](Triangle2D& t)
										{
											return t.valid;
										}), end(_triangles));

		for (auto e1 = begin(polygon); e1 != end(polygon); ++e1)
		{
			for (auto e2 = e1 + 1; e2 != end(polygon); ++e2)
			{
				if (EqualEdges(*e1, *e2))
				{
					e1->valid = true;
					e2->valid = true;
				}
			}
		}

		polygon.erase(std::remove_if(begin(polygon), end(polygon), [](Edge2D& e)
									 {
										 return e.valid;
									 }), end(polygon));

		for (const auto& e : polygon)
			_triangles.push_back(Triangle2D(e.p1, e.p2, *p));

	}

	_triangles.erase(std::remove_if(begin(_triangles), end(_triangles), [p1, p2, p3](Triangle2D& t)
									{
										return TriContainsVertex(t, p1) || TriContainsVertex(t, p2) || TriContainsVertex(t, p3);
									}), end(_triangles));

	//for (const auto t : _triangles)
	//{
	//	_edges.push_back(Edge<float>{*t.a, * t.b});
	//	_edges.push_back(Edge<float>{*t.b, * t.c});
	//	_edges.push_back(Edge<float>{*t.c, * t.a});
	//}

	return _triangles;
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
	for (int i = 0; i < 3; ++i)
	{
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

bool Triangle2D::operator==(const Triangle2D& t) const
{
	return	(this->p1 == t.p1 || this->p1 == t.p2 || this->p1 == t.p3) &&
		(this->p2 == t.p1 || this->p2 == t.p2 || this->p2 == t.p3) &&
		(this->p3 == t.p1 || this->p3 == t.p2 || this->p3 == t.p3);
}

Edge2D::Edge2D(const glm::vec2& v1, const glm::vec2& v2) : p1(v1), p2(v2)
{
}

bool Edge2D::operator==(const Edge2D& e) const
{
	return (this->p1 == e.p1 && (this->p2) == e.p2) ||
		(this->p1 == e.p2 && (this->p2) == e.p1);
}
