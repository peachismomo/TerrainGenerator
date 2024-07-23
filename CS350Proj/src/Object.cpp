#include "Object.h"
#include "MeshLoader.h"
#include "OGLWrapper.h"
#include <glm/ext/matrix_transform.hpp>

#include "Engine.h"
#include <CustomMath.h>
#include <limits>

#define BIT(x) (1U << x)

void Object::Render(Camera& camera, unsigned int shdr_id, MeshLoader& mesh_lib)
{
	Transform();

	if (m_mesh_name == "None")
		m_mesh = nullptr;

	if (m_mesh)
	{
		OGLWRAPPER::BindVAO(m_mesh->vao);
		OGLWRAPPER::UseShader(shdr_id);

		camera.SetUniforms(shdr_id);
		OGLWRAPPER::SetIntUniform(shdr_id, "u_debug_flag", 0);
		OGLWRAPPER::SetMat4Uniform(shdr_id, "u_mdl", m_transform);

		for (auto& entry : m_mesh->m_mesh_entries)
			OGLWRAPPER::DrawElementsBaseVertex(GL_TRIANGLES, entry.indices_cnt, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * entry.base_index), entry.base_vertex);
	}
	else if (!m_active)
	{
		OGLWRAPPER::BindVAO(mesh_lib.GetMesh("debug_point")->vao);
		OGLWRAPPER::UseShader(shdr_id);
		camera.SetUniforms(shdr_id);
		OGLWRAPPER::SetIntUniform(shdr_id, "u_debug_flag", 1);
		OGLWRAPPER::SetFloat3Uniform(shdr_id, "u_debug_clr", glm::vec3(0.f, 0.f, 1.f));

		OGLWRAPPER::SetMat4Uniform(shdr_id, "u_mdl", m_transform);
		OGLWRAPPER::SetPointSize(10.f);
		OGLWRAPPER::DrawArrays(GL_POINTS, 0, 1);
		OGLWRAPPER::SetPointSize(1.f);
	}

	OGLWRAPPER::UseShader();
	OGLWRAPPER::BindVAO();

	if (!engine.GetEditor().m_render_bv)
		return;

	glm::vec3 wireframe_clr = (m_intersection == 1) ? glm::vec3(1.f, 0.f, 0.f) : (m_intersection == 0 ? glm::vec3(0.f, 1.f, 0.f) : glm::vec3(1.f, 1.f, 0.f));

	if (!engine.GetEditor().m_type)
	{
		if (engine.GetEditor().m_ellipse)
			DebugRenderer::RenderDebugSphere(mesh_lib.GetMesh("debug_icosphere"), m_sphere.position, m_ellipse.m_rot_mtx, m_ellipse.m_half_axes, shdr_id, camera, wireframe_clr);
		else
			DebugRenderer::RenderDebugSphere(mesh_lib.GetMesh("debug_icosphere"), m_sphere.position, glm::vec3(0.f), glm::vec3(m_sphere.radius), shdr_id, camera, wireframe_clr);
	}

	if (engine.GetEditor().m_type)
		DebugRenderer::RenderDebugAABB(mesh_lib.GetMesh("debug_cube"), m_aabb.center, m_aabb.half_extent, shdr_id, camera, wireframe_clr);

	//if (IsAttribActive(ATTRIB_RAY))
	//{
	//	Point3D p2 = m_ray.origin.p + (m_ray.direction.p * .2f);
	//	// TODO: SOME LINE CALC
	//	DebugRenderer::RenderDebugRay(mesh_lib.GetMesh("debug_line"), m_ray.origin, p2, engine.GetRenderer().GetLineShdrID(), camera);
	//}

	//if (IsAttribActive(ATTRIB_PLANE))
	//{
	//	DebugRenderer::RenderDebugPlane(mesh_lib.GetMesh("debug_plane"), m_plane.normal, shdr_id, camera, wireframe_clr, false, 4.f);
	//}

	//if (IsAttribActive(ATTRIB_TRIANGLE))
	//{
	//	DebugRenderer::RenderDebugTriangle(mesh_lib.GetMesh("debug_triangle"), m_triangle.p1, m_triangle.p2, m_triangle.p3, engine.GetRenderer().GetLineShdrID(), camera, wireframe_clr);
	//}
}

void Object::SetMesh(Mesh* mesh)
{
	m_mesh = mesh;
	// m_sphere.radius = DebugRenderer::CalculateSphereRadius(m_mesh);
	// m_aabb.half_extent.p = DebugRenderer::CalculateAABBDimensions(m_mesh);
}

void Object::SetMeshName(std::string mesh_name)
{
	m_mesh_name = mesh_name;
}

void Object::SetMesh(std::string name)
{
	m_mesh = engine.GetRenderer().GetMeshLoader().GetMesh(name);
	// m_sphere.radius = DebugRenderer::CalculateSphereRadius(m_mesh);
	m_mesh_name = name;
	// m_aabb.half_extent.p = DebugRenderer::CalculateAABBDimensions(m_mesh);
}

void Object::SetAttribActive(ATTRIBUTES attrib)
{
	m_active |= BIT(attrib);
}

void Object::SetAttribInactive(ATTRIBUTES attrib)
{
	m_active &= ~BIT(attrib);
}

void Object::SetAttribsActive(unsigned int attribs)
{
	m_active = attribs;
}

void Object::ClearAttribs()
{
	m_active = 0;
}

bool Object::IsAttribActive(ATTRIBUTES attrib) const
{
	return (m_active & BIT(attrib)) != 0;
}

void Object::CalculateBoundingSphere(METHOD method)
{
	switch (method)
	{
		case M_PCA:
		{
			CalculatePCA();
			break;
		}
		case M_RITTERS:
		{
			CalculateRitters();
			break;
		}
		case M_LARSON:
		{
			CalculateLarsons();
			break;
		}
		default:
			break;
	}
}

void Object::CalculateAABB()
{
	std::vector<glm::vec3> vertices = GetVertices();
	glm::vec3 min_p = glm::vec3{ FLT_MAX };
	glm::vec3 max_p = glm::vec3{ -FLT_MAX };

	for (auto& vertex : vertices)
	{
		min_p.x = CMIN(min_p.x, vertex.x);
		min_p.y = CMIN(min_p.y, vertex.y);
		min_p.z = CMIN(min_p.z, vertex.z);

		max_p.x = CMAX(max_p.x, vertex.x);
		max_p.y = CMAX(max_p.y, vertex.y);
		max_p.z = CMAX(max_p.z, vertex.z);
	}

	glm::vec3 center = (min_p + max_p) * 0.5f;
	glm::vec3 half_extents = (max_p - min_p) * 0.5f;

	m_aabb.center = center;
	m_aabb.half_extent = half_extents;
}

void Object::Transform()
{
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scale.p);
	glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), m_rotation.p.x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), m_rotation.p.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), m_rotation.p.z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), m_translate.p);

	glm::mat4 rotation = rotateZ * rotateY * rotateX;
	m_transform = translate * rotation * scale;
}

void Object::CalculatePCA()
{
	std::vector<glm::vec3> points = GetVertices();

	if (points.empty())
		return;

	glm::vec3 mean;
	glm::mat3 eigenvectors;
	glm::vec3 eigenvalues;
	glm::mat3 covariance = PCA(points, mean, eigenvectors, eigenvalues);

	glm::vec3 e;
	int maxc = 0;
	float maxf, maxe = abs(covariance[0][0]);
	if ((maxf = abs(covariance[1][1])) > maxe) maxc = 1, maxe = maxf;
	if ((maxf = abs(covariance[2][2])) > maxe) maxc = 2, maxe = maxf;
	e[0] = eigenvectors[0][maxc];
	e[1] = eigenvectors[1][maxc];
	e[2] = eigenvectors[2][maxc];

	int imin, imax;
	FindExtremalPoints(e, points, &imin, &imax);
	glm::vec3 minpt = points[imin];
	glm::vec3 maxpt = points[imax];

	float dist = sqrtf(glm::dot(maxpt - minpt, maxpt - minpt));
	m_sphere.radius = dist * 0.5f;
	m_sphere.position = (minpt + maxpt) * 0.5f;

	m_ellipse.m_half_axes = eigenvalues;
	m_ellipse.m_rot_mtx = eigenvectors;

	for (const auto& point : points)
	{
		float d_sq = CMATH::DistanceSquared(m_sphere.position, point);
		if (d_sq > m_sphere.radius * m_sphere.radius)
		{
			float distance = glm::sqrt(d_sq);
			m_sphere.radius = (m_sphere.radius + distance) * 0.5f;
			m_sphere.position = m_sphere.position.p + ((point - m_sphere.position.p) * ((distance - m_sphere.radius) / distance));
		}
	}
}

void Object::CalculateLarsons()
{
	std::vector<glm::vec3> points = GetVertices();

	if (points.size() < 2)
		return;

	glm::vec3 min_x = {  FLT_MAX,	 0,			 0			};
	glm::vec3 max_x = { -FLT_MAX,	 0,			 0			};
	glm::vec3 min_y = {  0,			 FLT_MAX,	 0			};
	glm::vec3 max_y = {  0,			-FLT_MAX,	 0			};
	glm::vec3 min_z = {  0,			 0,			 FLT_MAX	};
	glm::vec3 max_z = {  0,			 0,			-FLT_MAX	};

	for (const auto& p : points)
	{
		if (p.x < min_x.x) min_x = p;
		if (p.x > max_x.x) max_x = p;
		if (p.y < min_y.y) min_y = p;
		if (p.y > max_y.y) max_y = p;
		if (p.z < min_z.z) min_z = p;
		if (p.z > max_z.z) max_z = p;
	}

	std::vector<glm::vec3> extreme_pts = { min_x, max_x, min_y, max_y, min_z, max_z };

	glm::vec3 furthest_pt_a, furthest_pt_b;
	float max_dist = -1.0;

	for (const auto& a : extreme_pts)
	{
		for (const auto& b : extreme_pts)
		{
			float dist = distance(a, b);
			if (dist > max_dist)
			{
				max_dist = dist;
				furthest_pt_a = a;
				furthest_pt_b = b;
			}
		}
	}

	glm::vec3 center = (furthest_pt_a + furthest_pt_b) * 0.5f;
	float radius = max_dist / 2.f;

	for (const auto& p : points)
	{
		float dist_to_center = distance(center, p);
		if (dist_to_center > radius)
		{
			float r_prime = (radius + dist_to_center) / 2.f;
			glm::vec3 direction = { (p.x - center.x) / dist_to_center, (p.y - center.y) / dist_to_center, (p.z - center.z) / dist_to_center };

			center = {
				center.x + direction.x * (r_prime - radius),
				center.y + direction.y * (r_prime - radius),
				center.z + direction.z * (r_prime - radius)
			};
			radius = r_prime;
		}
	}

	m_sphere.position = center;
	m_sphere.radius = radius;
}

void Object::CalculateRitters()
{
	std::vector<glm::vec3> points = GetVertices();

	if (points.empty())
		return;

	glm::vec3 a = points[0];

	glm::vec3 b = a;
	float max_dist = 0.0f;
	for (const auto& p : points)
	{
		float distanceSq = CMATH::DistanceSquared(a, p);
		if (distanceSq > max_dist)
		{
			max_dist = distanceSq;
			b = p;
		}
	}

	glm::vec3 c = b;
	max_dist = 0.0f;
	for (const auto& p : points)
	{
		float dist_sq = CMATH::DistanceSquared(b, p);
		if (dist_sq > max_dist)
		{
			max_dist = dist_sq;
			c = p;
		}
	}

	glm::vec3 center = (b + c) * 0.5f;
	float radius = CMATH::Distance(b, c) * 0.5f;
	float radius_sq = radius * radius;

	for (const auto& point : points)
	{
		float distanceSq = CMATH::DistanceSquared(center, point);
		if (distanceSq > radius_sq)
		{
			float distance = std::sqrt(distanceSq);
			float new_radius = (radius + distance) * 0.5f;
			float k = (new_radius - radius) / distance;
			radius = new_radius;
			radius_sq = radius * radius;
			center += k * (point - center);
		}
	}

	m_sphere = BoundingSphere{ center, radius };
}

std::vector<glm::vec3> Object::GetVertices()
{
	std::vector<glm::vec3> vertices;

	if (m_mesh_name.empty())
		return vertices;

	for (auto& vertex : m_mesh->m_position_buffer)
		vertices.emplace_back(m_transform * glm::vec4(vertex, 1.f));

	return vertices;
}

glm::vec3 Object::ComputeMean(std::vector<glm::vec3> const& points)
{
	glm::vec3 mean(0.0f);
	for (const auto& point : points)
		mean += point;
	return mean / static_cast<float>(points.size());
}

glm::mat3 Object::ComputeCovarianceMatrix(std::vector<glm::vec3> const& points, glm::vec3 const& mean)
{
	glm::mat3 covar(0.0f);
	std::vector<glm::vec3> pt = points;
	float oon = 1.0f / (float)pt.size();
	glm::vec3 c = glm::vec3(0.0f, 0.0f, 0.0f);
	float e00, e11, e22, e01, e02, e12;
	for (int i = 0; i < pt.size(); i++)
		c += pt[i];
	c *= oon;

	e00 = e11 = e22 = e01 = e02 = e12 = 0.0f;
	for (int i = 0; i < pt.size(); i++)
	{

		glm::vec3 p = pt[i] - c;

		e00 += p.x * p.x;
		e11 += p.y * p.y;
		e22 += p.z * p.z;
		e01 += p.x * p.y;
		e02 += p.x * p.z;
		e12 += p.y * p.z;
	}

	covar[0][0] = e00 * oon;
	covar[1][1] = e11 * oon;
	covar[2][2] = e22 * oon;
	covar[0][1] = covar[1][0] = e01 * oon;
	covar[0][2] = covar[2][0] = e02 * oon;
	covar[1][2] = covar[2][1] = e12 * oon;

	return covar;
}

glm::mat3 Object::PCA(std::vector<glm::vec3> const& points, glm::vec3& mean, glm::mat3& eigenvectors, glm::vec3& eigenvalues)
{
	mean = ComputeMean(points);
	glm::mat3 covariance = ComputeCovarianceMatrix(points, mean);

	Jacobi(covariance, eigenvectors);
	for (int i{}; i < 3; ++i)
		eigenvalues[i] = covariance[i][i];
	return covariance;
}

void Object::Jacobi(glm::mat3& a, glm::mat3& v)
{
	int i{}, j{}, n{}, p{}, q{};
	float prevoff{}, c{}, s{};
	glm::mat3 J{}, b{}, t{};

	for (i = 0; i < 3; i++)
	{
		v[i][0] = v[i][1] = v[i][2] = 0.0f;
		v[i][i] = 1.0f;
	}
	const int MAX_ITERATIONS = 100;
	for (n = 0; n < MAX_ITERATIONS; n++)
	{
		p = 0; q = 1;
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				if (i == j) 
					continue;
				if (abs(a[i][j]) > abs(a[p][q]))
				{
					p = i;
					q = j;
				}
			}
		}

		SymSchur2(a, p, q, c, s);
		for (i = 0; i < 3; i++)
		{
			J[i][0] = J[i][1] = J[i][2] = 0.0f;
			J[i][i] = 1.0f;
		}
		J[p][p] = c; J[p][q] = s;
		J[q][p] = -s; J[q][q] = c;

		v = v * J;

		a = (glm::transpose(J) * a) * J;

		float off = 0.0f;
			for (i = 0; i < 3; i++)
			{
				for (j = 0; j < 3; j++)
				{
					if (i == j) continue;
					off += a[i][j] * a[i][j];
				}
			}

		if (n > 2 && off >= prevoff)
			return;
		prevoff = off;
	}
}

void Object::SymSchur2(glm::mat3 const& a, int p, int q, float& c, float& s)
{
	if (abs(a[p][q]) > 0.0001f)
	{
		float r = (a[q][q] - a[p][p]) / (2.0f * a[p][q]);
		float t;
		if (r >= 0.0f)
			t = 1.0f / (r + sqrtf(1.0f + r * r));
		else
			t = -1.0f / (-r + sqrtf(1.0f + r * r));
		c = 1.0f / sqrtf(1.0f + t * t);
		s = t * c;
	}
	else
	{
		c = 1.0f;
		s = 0.0f;
	}
}

void Object::FindExtremalPoints(glm::vec3 dir, std::vector<glm::vec3> const& pts, int* imin, int* imax)
{
	float minproj = FLT_MAX, maxproj = -FLT_MAX;
	for (size_t i = 0; i < pts.size(); i++)
	{
		float proj = glm::dot(pts[i], dir);
		if (proj < minproj)
		{
			minproj = proj;
			*imin = static_cast<int>(i);
		}
		if (proj > maxproj)
		{
			maxproj = proj;
			*imax = static_cast<int>(i);
		}
	}
}
