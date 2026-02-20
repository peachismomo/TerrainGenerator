#include "Terrain.h"
#include <Utils.h>
#include "PoissonDiskSampling.h"
#include "Perlin.h"
#include "delaunay.h"
#include "vector2.h"
#include <chrono>
#include <Primitives.h>

static unsigned int GenerateSeed()
{
	auto now = std::chrono::high_resolution_clock::now();
	auto duration = now.time_since_epoch();
	return static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}

static glm::vec3 GreenToWhite(float t)
{
	if (t < 0.f) t = 0.f;
	if (t > 1.f) t = 1.f;

	float r = t;
	float b = t;

	return glm::vec3(r, 1.f, b);
}

static glm::vec3 BlueToBlack(float t)
{
	if (t < 0.f) t = 0.f;
	if (t > 1.f) t = 1.f;

	float b = 1.f - t;

	return glm::vec3(0.f, 0.f, b);
}

static glm::vec3 InterpolateColor(const glm::vec3& color1, const glm::vec3& color2, float t)
{
	return (1.f - t) * color1 + t * color2;
}

static glm::vec3 GetColor(float value)
{
	static glm::vec3 brown = glm::vec3(194.f / 255.f, 178.f / 255.f, 128.f / 255.f);
	static glm::vec3 green = glm::vec3(0.f, 255.f / 255.f, 0.f);
	static glm::vec3 white = glm::vec3(255.f / 255.f, 255.f / 255.f, 255.f / 255.f);

	if (value <= 0.2f)
	{
		// Interpolate from brown to green
		float t = value / 0.2f;
		return InterpolateColor(brown, green, t);
	}
	else if (value <= 0.5f)
	{
		// Interpolate from green to white
		float t = (value - 0.2f) / 0.3f;
		return InterpolateColor(green, white, t);
	}
	else return white;
}

static glm::vec3 ComputeFaceNormal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
{
	glm::vec3 edge1 = v1 - v0;
	glm::vec3 edge2 = v2 - v0;
	glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
	return normal;
}

static void CalculateVertexNormals(std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& vertices)
{
	normals.clear();
	// Initialize vertex normals to zero
	for (auto& vertex : vertices)
		normals.emplace_back(glm::vec3(0.f));

	// Accumulate face normals into vertex normals
	for (size_t i{}; i < vertices.size(); i += 3)
	{
		glm::vec3 normal = ComputeFaceNormal(vertices[i], vertices[i + 1], vertices[i + 2]);

		for (int j = 0; j < 3; ++j)
			normals[i + j] += normal;
	}

	// Normalize the vertex normals
	for (auto& vertex : normals)
		vertex = glm::normalize(vertex);
}

void Terrain::GeneratePoints(unsigned int seed,unsigned int no_pts, glm::vec3 map_scale, unsigned int perlin_oct, float perlin_persistance, float perlin_freq)
{
	m_terrain_vtx.clear();
	m_nml.clear();
	m_clrs.clear();
	m_indices.clear();

	std::vector<glm::vec2> points = Poisson::GeneratePoissonPoints(no_pts, seed);

	float min_x = std::numeric_limits<float>::max();
	float max_x = std::numeric_limits<float>::lowest();
	float min_y = std::numeric_limits<float>::max();
	float max_y = std::numeric_limits<float>::lowest();

	for (const auto& p : points)
	{
		if (p.x < min_x) min_x = p.x;
		if (p.x > max_x) max_x = p.x;
		if (p.y < min_y) min_y = p.y;
		if (p.y > max_y) max_y = p.y;
	}

	for (auto& p : points)
	{
		float normalized_x = 2.f * (p.x - min_x) / (max_x - min_x) - 1.f;
		float normalized_y = 2.f * (p.y - min_y) / (max_y - min_y) - 1.f;
		p.x = normalized_x * map_scale.x;
		p.y = normalized_y * map_scale.z;

		m_poisson_points.emplace_back(glm::vec3(p.x, 0.f, p.y));
	}

	std::vector<Triangle2D> triangles = TESTS::Triangulate(points);
	for (auto& tri : triangles)
	{
		m_terrain_vtx.emplace_back(tri.p1.x, 0.f, tri.p1.y);
		m_terrain_vtx.emplace_back(tri.p2.x, 0.f, tri.p2.y);
		m_terrain_vtx.emplace_back(tri.p3.x, 0.f, tri.p3.y);
	}

	// Heightmap
	const siv::PerlinNoise::seed_type perlin_seed = seed;
	const siv::PerlinNoise perlin{ perlin_seed };

	for (auto& p : m_terrain_vtx)
	{
		p.y = (float)perlin.octave2D_11Smooth((double)p.x, (double)p.z, perlin_oct, perlin_persistance, perlin_freq);

		if (p.y < 0.f)
			m_clrs.emplace_back(BlueToBlack(p.y));
		else
			m_clrs.emplace_back(GetColor(p.y));

		p.y *= map_scale.y;
	}

	CalculateVertexNormals(m_nml, m_terrain_vtx);
}