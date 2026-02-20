#ifndef UTILS_H
#define UTILS_H

#include <includes.h>

#include <random>

namespace UTILS
{
	template <typename T>
	struct RNG
	{
		std::default_random_engine dre;
		std::uniform_real_distribution<T> urdf;

		RNG(T min, T max) : urdf(min, max), dre(std::random_device{}()) {}
		RNG(T min, T max, unsigned int seed) : urdf(min, max), dre(seed) {}
		void SetSeed(unsigned int seed) { dre.seed(seed); }
		T getRNG() { return urdf(dre); }
	};
}

#endif // !UTILS_H
