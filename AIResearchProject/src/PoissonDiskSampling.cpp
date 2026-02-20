#include "PoissonDiskSampling.h"
#include <glm/gtx/norm.hpp> 

inline float Poisson::DefaultPRNG::randomFloat()
{
	seed_ *= 521167;
	uint32_t a = (seed_ & 0x007fffff) | 0x40000000;
	return 0.5f * (*((float*)&a) - 2.0f);
}

inline uint32_t Poisson::DefaultPRNG::randomInt(uint32_t maxInt)
{
	return uint32_t(randomFloat() * maxInt);
}

inline uint32_t Poisson::DefaultPRNG::getSeed() const
{
	return seed_;
}

bool Poisson::InRectangle(glm::vec2 point)
{
	return point.x >= 0.f && point.y >= 0.f && point.x <= 1.f && point.y <= 1.f;
}

Poisson::GridPoint Poisson::ImageToGrid(const glm::vec2& P, float cellSize)
{
    return GridPoint((int)(P.x / cellSize), (int)(P.y / cellSize));
}

Poisson::Grid::Grid(int w, int h, float cellSize)
    : w_(w), h_(h), cellSize_(cellSize)
{
    grid_.resize(h_);
    for (auto& i : grid_)
        i.resize(w);
}

void Poisson::Grid::insert(const glm::vec2& p)
{
    const GridPoint g = ImageToGrid(p, cellSize_);
    grid_[g.x][g.y] = p;
}

bool Poisson::Grid::IsInNeighbourhood(const glm::vec2& point, float min_dist, float cell_sz)
{
    const GridPoint g = ImageToGrid(point, cell_sz);
    static const int D = 5;
	for (int i{ g.x - D }; i <= g.x + D; i++)
		for (int j{ g.y - D }; j <= g.y + D; j++)
            if (i >= 0 && i < w_ && j >= 0 && j < h_)
            {
                glm::vec2 P = grid_[i][j];
                if (glm::distance2(P, point) < min_dist)
                    return true;
            }

    return false;
}

std::vector<glm::vec2> Poisson::GeneratePoissonPoints(uint32_t num_p, unsigned int rng, uint32_t newPointsCount, float min_dist)
{
	Poisson::DefaultPRNG gen = Poisson::DefaultPRNG(rng);

	num_p *= 2;
	if (min_dist < 0.0f)
		min_dist = sqrt(float(num_p)) / float(num_p);

	num_p = static_cast<int>(0.785398163397448309616 * num_p);

	std::vector<glm::vec2> out_points;
	std::vector<glm::vec2> temp_points;

	if (!num_p)
		return out_points;

	float cell_sz = min_dist / 1.414214f;
	Grid grid((int)ceil(1.f / cell_sz), (int)ceil(1.f / cell_sz), cell_sz);

	glm::vec2 f_p{};

	f_p = glm::vec2(gen.randomFloat(), gen.randomFloat());
	while (!InRectangle(f_p))
		f_p = glm::vec2(gen.randomFloat(), gen.randomFloat());

	temp_points.emplace_back(f_p);
	out_points.emplace_back(f_p);
	grid.insert(f_p);

	float min_dist_sq = min_dist * min_dist;

	while (!temp_points.empty() && out_points.size() <= num_p)
	{
		glm::vec2 point = PopRandom(temp_points, gen);

		for (uint32_t i = 0; i < newPointsCount; i++)
		{
			glm::vec2 n_p = GenerateRandomPointAround(point, min_dist, gen);

			if (InRectangle(n_p) && !grid.IsInNeighbourhood(n_p, min_dist_sq, cell_sz))
			{
				temp_points.emplace_back(n_p);
				out_points.emplace_back(n_p);
				grid.insert(n_p);
				continue;
			}
		}
	}
	return out_points;
}

glm::vec2 Poisson::GenerateRandomPointAround(const glm::vec2& p, float min_dist, DefaultPRNG& seed)
{
	float radius	= min_dist * (seed.randomFloat() + 1.0f);
	float angle		= 2.f * 3.141592653589f * seed.randomFloat();

	return glm::vec2(p.x + radius * cos(angle), p.y + radius * sin(angle));
}

glm::vec2 Poisson::PopRandom(std::vector<glm::vec2>& points, DefaultPRNG& seed)
{
	int idx		= static_cast<int>(seed.randomInt(static_cast<int>(points.size()) - 1));
	glm::vec2 p = points[idx];

	points.erase(points.begin() + idx);

	return p;
}