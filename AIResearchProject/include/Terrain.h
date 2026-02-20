#ifndef TERRAIN_H
#define TERRAIN_H

#include <includes.h>

class Terrain
{
public:
	void GeneratePoints(unsigned int seed = 1234, unsigned int no_pts = 10000, glm::vec3 map_scale = glm::vec3(10.f), unsigned int perlin_oct = 4, float perlin_persistance = 0.5f, float perlin_freq = 10.f);

	std::vector<glm::vec3> GetVtx()  const { return m_terrain_vtx; }
	std::vector<glm::vec3> GetNml()  const { return m_nml; }
	std::vector<glm::vec3> GetClr()  const { return m_clrs; }
	std::vector<unsigned int> GetIndices()  const { return m_indices; }
	std::vector<glm::vec3> GetPoisson() const { return m_poisson_points; }

private:
	std::vector<glm::vec3> m_poisson_points;
	std::vector<glm::vec3> m_terrain_vtx;
	std::vector<glm::vec3> m_nml;
	std::vector<glm::vec3> m_clrs;
	std::vector<unsigned int> m_indices;
};

#endif // !TERRAIN_H
