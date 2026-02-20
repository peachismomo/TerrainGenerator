#ifndef MESHLOADER_H
#define MESHLOADER_H

#include "includes.h"
#include "Terrain.h"

struct Mesh
{
	unsigned int vao;
	unsigned int pos_vbo;
	unsigned int nml_vbo;
	unsigned int ebo_vbo;
	unsigned int clr_vbo;

	std::vector<glm::vec3>		m_position_buffer{};
	std::vector<glm::vec2>		m_2d_position_buffer{};
	std::vector<glm::vec2>		m_2d_tex_buffer{};
	std::vector<glm::vec3>		m_normal_buffer{};
	std::vector<unsigned int>	m_indices{};

	struct MeshEntry
	{
		unsigned int indices_cnt{};
		unsigned int base_vertex{};
		unsigned int base_index{};
		unsigned int mat_index;
	};

	std::vector<MeshEntry> m_mesh_entries{};
};

class MeshLoader
{
public:
	void LoadMesh(std::string path);
	void LoadTerrain(Terrain& terrain, unsigned int seed = 1234, unsigned int no_pts = 10000, glm::vec3 map_scale = glm::vec3(10.f), unsigned int perlin_oct = 4, float perlin_persistance = 0.5f, float perlin_freq = 10.f);
	Mesh* GetMesh(std::string name);
	void LoadDebugMesh();
	std::unordered_map<std::string, Mesh>& GetMeshes() { return m_meshes; }

private:
	void SubdivideIcoSphere(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, int depth, std::vector<glm::vec3>& vertices);

	std::unordered_map<std::string, Mesh> m_meshes;
};

#endif // !MESHLOADER_H
