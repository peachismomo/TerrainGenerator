#ifndef POISSON_H
#define POISSON_H

#include <stdint.h>
#include <vector>

#include "includes.h"
#include "Utils.h"

namespace Poisson
{
	class DefaultPRNG
	{
	public:
		DefaultPRNG() = default;
		explicit DefaultPRNG(unsigned int seed) : seed_(seed) {}
		inline float randomFloat();
		inline uint32_t randomInt(uint32_t maxInt);
		inline uint32_t getSeed() const;

	private:
		uint32_t seed_ = 7133167;
	};

	struct GridPoint
	{
		GridPoint() = delete;
		GridPoint(int X, int Y) : x(X), y(Y) {}
		int x;
		int y;
	};

	struct Grid
	{
		Grid(int w, int h, float cellSize);
		void insert(const glm::vec2& p);
		bool IsInNeighbourhood(const glm::vec2& point, float minDist, float cellSize);

	private:
		int w_;
		int h_;
		float cellSize_;
		std::vector<std::vector<glm::vec2>> grid_;
	};

	glm::vec2 PopRandom(std::vector<glm::vec2>& points, DefaultPRNG& rng);

	glm::vec2 GenerateRandomPointAround(const glm::vec2& p, float minDist, DefaultPRNG& rng);

	bool InCircle(glm::vec2 point);
	bool InRectangle(glm::vec2 point);
	GridPoint ImageToGrid(const glm::vec2& P, float cellSize);

	std::vector<glm::vec2> GeneratePoissonPoints(uint32_t numPoints,
												 unsigned int rng,
												 uint32_t newPointsCount = 30,
												 float minDist = -1.f);
} // namespace PoissonGenerator

#endif // !POISSON_H