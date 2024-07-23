#ifndef UTILS_H

#include <includes.h>

#include <random>

class Perlin
{
public:
	Perlin(unsigned int seed = 123) : m_seed(seed)
	{
		Generate3Primes();
	}

	float GenPerlin(float x, float y);

private:
	glm::vec2 RandomGradient(int ix, int iy) const;
	float DotGridGradient(int ix, int iy, float x, float y);
	float Interpolate(float p1, float p2, float w);

	bool IsPrime(uint64_t n)
	{
		if (n <= 1) return false;
		if (n <= 3) return true;
		if (n % 2 == 0 || n % 3 == 0) return false;
		for (uint64_t i = 5; i * i <= n; i += 6)
		{
			if (n % i == 0 || n % (i + 2) == 0) return false;
		}
		return true;
	}

	unsigned int RandomPrime(uint64_t seed)
	{
		std::mt19937_64 rng(seed); // 64-bit Mersenne Twister engine
		std::uniform_int_distribution<uint64_t> dist(1ULL << 61, ~(0ULL >> 1)); // Large range

		uint64_t num = dist(rng);
		while (!IsPrime(num))
		{
			num = dist(rng);
		}
		return static_cast<unsigned int>(num);
	}

	void Generate3Primes()
	{
		prime1 = RandomPrime(m_seed);
		prime2 = RandomPrime(m_seed);
		prime3 = RandomPrime(m_seed);
	}

	unsigned int prime1{}, prime2{}, prime3{};
	unsigned int m_seed{};
};

class PoissonDiskSampling
{
public:
	PoissonDiskSampling(void) {}
	~PoissonDiskSampling(void) {}

	PoissonDiskSampling(int p_width, int p_height, float p_min_dist, int p_point_count);

	std::vector<std::pair<float, float>> Generate();

	static int GridIndex(glm::vec3 p, float cell_size, int map_width)
	{
		int x_index = static_cast<int>(p.x / cell_size);
		int y_index = static_cast<int>(p.z / cell_size);

		return x_index + y_index * map_width;
	};

private:
	std::vector<std::vector<glm::vec3*>>	m_grid{};
	std::vector<glm::vec3>					m_process{};
	std::vector<std::pair<float, float>>	m_sample{};

	int		m_width{};
	int		m_height{};
	float	m_min_dist{};
	int		m_point_count{};
	float	m_cell_size{};
	int		m_grid_width{};
	int		m_grid_height{};

	glm::vec3 GeneratePointAround(glm::vec3 p_point) const;
	bool InRectangle(glm::vec3 p_point) const;
	bool InNeighbourhood(glm::vec3 p_point);
	std::vector<glm::vec3*> GetCellsAround(glm::vec3 p_point);
};

namespace UTILS
{
	template <typename T>
	struct RNG
	{
		std::default_random_engine dre;
		std::uniform_real_distribution<T> urdf;

		RNG(T min, T max) : urdf(min, max), dre(std::random_device{}()) {}
		RNG(T min, T max, unsigned int seed) : urdf(min, max), dre(seed) {}
		T getRNG() { return urdf(dre); }
	};

	// PERLIN NOISE

	// VORONOI DIAGRAM	
}

#endif // !UTILS_H
