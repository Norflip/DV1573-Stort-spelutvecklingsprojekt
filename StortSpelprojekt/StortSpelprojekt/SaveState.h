#pragma once
#include <functional>
#include "Random.h"

struct SaveState
{
	int seed;
	size_t segment;
	std::vector<int> m_generatedSeeds;

	inline void GenerateSeeds(size_t count)
	{
		int baseSeed = seed ^ std::hash<int>()(segment);
		Random::SetSeed(baseSeed);
		m_generatedSeeds.clear();

		for (size_t i = 0; i < count; i++)
		{
			int seed = Random::Range(-100000, 100000);
			m_generatedSeeds.push_back(seed);
			Random::SetSeed(seed);
		}
	}

	inline int GetSeed(size_t index) const
	{
		return m_generatedSeeds[index % m_generatedSeeds.size()];
	}
};