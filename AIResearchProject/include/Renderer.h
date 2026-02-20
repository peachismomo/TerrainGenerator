#ifndef RENDERER_H
#define RENDERER_H

#include "includes.h"
#include "Camera.h"
#include "Object.h"
#include "MeshLoader.h"
#include "BVH.h"
#include "Terrain.h"

class Renderer
{
public:
	void Init();
	void Update();
	void End();

	std::vector<Object*>&	GetObjects()			{ return m_objects; }
	MeshLoader&				GetMeshLoader()			{ return m_mesh_loader; }
	Camera&					GetCamera()				{ return m_camera; }
	unsigned int			GetLineShdrID() const	{ return m_line_shdr_id; }

	BVHTopDown&				GetBVHTopDown()			{ return m_BVH_topdown; }
	BVHBotUp&				GetBVHBotUp()			{ return m_BVH_botup; }

	Terrain terrain;
	void GenerateTerrain(unsigned int seed, unsigned int no_pts, glm::vec3 map_scale, unsigned int perlin_oct, float perlin_persistance, float perlin_freq);

private:
	void RenderScene(Camera& camera, bool thicken = false);
	void RenderBVH(Camera& camera, BVHNode* root, BVTYPE type, int depth = 0, bool thicken = false);

	Camera					m_camera;
	std::vector<Object*>	m_objects;
	unsigned int			m_shdr_id;
	unsigned int			m_line_shdr_id;
	MeshLoader				m_mesh_loader;
	
	Camera					m_map_camera;
	unsigned int			m_map_framebuffer;
	unsigned int			m_map_texture;
	unsigned int			m_map_depth;
	unsigned int			m_map_shdr_id;

	BVHTopDown				m_BVH_topdown;
	BVHBotUp				m_BVH_botup;
};

namespace DebugRenderer
{
	void RenderDebugSphere(Mesh const* mesh, Point3D const& position, Point3D const& rotation, Point3D const& scale, unsigned int shdr_id, Camera const& camera, glm::vec3 clr = glm::vec3(0.f, 1.f, 0.f));
	void RenderDebugSphere(Mesh const* mesh, Point3D const& position, glm::mat3 const& rotation, Point3D const& scale, unsigned int shdr_id, Camera const& camera, glm::vec3 clr = glm::vec3(0.f, 1.f, 0.f));
	void RenderDebugAABB(Mesh const* mesh, Point3D const& position, Point3D const& scale, unsigned int shdr_id, Camera const& camera, glm::vec3 clr = glm::vec3(0.f, 1.f, 0.f));
	void RenderDebugPlane(Mesh const* mesh, glm::vec4 normal, unsigned int shdr_id, Camera const& camear, glm::vec3 clr, bool origin = false, float thickness = 1.f, int debug = 0);
	void RenderDebugRay(Mesh const* mesh, Point3D const& p1, Point3D const& p2, unsigned int shdr_id, Camera const& camera);
	void RenderDebugAxis(Mesh const* mesh, unsigned int shdr_id, Camera const& camera, float size = 1.f);
	void RenderDebugTriangle(Mesh const* mesh, Point3D const& p1, Point3D const& p2, Point3D const& p3, unsigned int shdr_id, Camera const& camera, glm::vec3 clr = glm::vec3(0.f, 1.f, 0.f));
	void RenderWireframe(glm::vec3 clr, unsigned int shdr_id, Mesh::MeshEntry const& entry);
	void RenderWireframeLines(glm::vec3 clr, unsigned int shdr_id, Mesh::MeshEntry const& entry);
	void RenderPoint(glm::vec3 clr, unsigned int shdr_id, Mesh::MeshEntry const& entry);

	float CalculateSphereRadius(Mesh* mesh);
	glm::vec3 CalculateAABBDimensions(Mesh* mesh);
};

#endif // !RENDERER_H
