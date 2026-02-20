#ifndef OBJECT_H
#define OBJECT_H

#include "Primitives.h"
#include "MeshLoader.h"
#include "Camera.h"

class Object
{
public:
	Object() {}
	void Render(Camera& camera, unsigned int shdr_id, MeshLoader& mesh_lib);

	void SetMesh(Mesh* mesh);
	void SetMeshName(std::string mesh_name);
	void SetMesh(std::string name);

	std::string GetMeshName() { return m_mesh_name; }

	Point3D&		GetPosition() { return m_position; }
	Plane&			GetPlane() { return m_plane; }
	Triangle&		GetTriangle() { return m_triangle; }
	BoundingSphere& GetSphere() { return m_sphere; }
	AABB&			GetAABB() { return m_aabb; }
	Ray&			GetRay() { return m_ray; }

	Point3D& GetTranslate() { return m_translate; }
	Point3D& GetRotation()	{ return m_rotation; }
	Point3D& GetScale()		{ return m_scale; }

	int& Intersection() { return m_intersection; }

	enum ATTRIBUTES
	{
		// ATTRIB_POSITION,
		ATTRIB_PLANE,
		ATTRIB_TRIANGLE,
		ATTRIB_SPHERE,
		ATTRIB_AABB,
		ATTRIB_RAY
	};

	void SetAttribActive(ATTRIBUTES attrib);
	void SetAttribInactive(ATTRIBUTES attrib);
	void SetAttribsActive(unsigned int attribs);
	void ClearAttribs();
	bool IsAttribActive(ATTRIBUTES attrib) const;
	unsigned int GetAttribs() const { return m_active; }

	enum METHOD
	{
		M_RITTERS,
		M_PCA,
		M_LARSON
	};

	void CalculateBoundingSphere(METHOD method);
	void CalculateAABB();
	void Transform();
	glm::mat3 PCA(std::vector<glm::vec3> const& points, glm::vec3& mean, glm::mat3& eigenvectors, glm::vec3& eigenvalues);

private:
	// Attributes
	Point3D			m_position{};	// 0
	Plane			m_plane{};		// 1
	Triangle		m_triangle{};	// 2
	BoundingSphere	m_sphere{};		// 3
	AABB			m_aabb{};		// 4
	Ray				m_ray{};		// 5
	BEllipse		m_ellipse;

	Mesh*			m_mesh{ nullptr };
	std::string		m_mesh_name{ "None" };

	Point3D		m_translate{ glm::vec3(0.f, 0.f, 0.f) };
	Point3D		m_rotation{ glm::vec3(0.f, 0.f, 0.f) };
	Point3D		m_scale{ glm::vec3(1.f, 1.f, 1.f) };

	glm::mat4		m_transform{};

	// bitset for active traits
	unsigned int	m_active{};

	int m_intersection{ false };

	void CalculatePCA();
	void CalculateLarsons();
	void CalculateRitters();

	std::vector<glm::vec3> GetVertices();

	// PCA
	glm::vec3 ComputeMean(std::vector<glm::vec3> const& points);
	glm::mat3 ComputeCovarianceMatrix(std::vector<glm::vec3> const& points, glm::vec3 const& mean);
	void Jacobi(glm::mat3& matrix, glm::mat3& v);
	void SymSchur2(glm::mat3 const& a, int p, int q, float& c, float& s);
	void FindExtremalPoints(glm::vec3 dir, std::vector<glm::vec3> const& pts, int* imin, int* imax);
};

#endif // !OBJECT_H
