#ifndef TERRAIN_H

#include <includes.h>

class Terrain
{
public:
	void GeneratePoints();

	std::vector<glm::vec3> GetVtx() { return m_terrain_vtx; }
	std::vector<glm::vec3> GetNml() { return m_nml; }

private:
	std::vector<glm::vec3> m_terrain_vtx;
	std::vector<glm::vec3> m_nml;
};

#endif // !TERRAIN_H
