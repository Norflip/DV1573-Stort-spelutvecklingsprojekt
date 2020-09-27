#pragma once
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include "ThirdParty\SimplexNoise.h"
#include "Chunk.h"
#include "Log.h"

namespace Noise
{
	struct Settings
	{
		int seed;

		float scale;
		size_t octaves;
		float persistance;
		float lacunarity;

		float xOffset;
		float yOffset;

		Settings(int seed)
		{
			this->seed = seed;
			this->scale = 20.0f;
			this->octaves = 4;
			this->persistance = 0.0f;
			this->lacunarity = 0.0f;

			std::uniform_real_distribution<float> distribution(-100000, 100000);
			std::default_random_engine rng;
			rng.seed(seed);
			
			this->xOffset = distribution(rng);
			this->yOffset = distribution(rng);
		}
	};

	inline float Sample(float x, float y, const Settings& settings, const SimplexNoise& noise)
	{
		return (noise.fractal(settings.octaves, settings.xOffset + x, settings.yOffset + y) + 1.0f) / 2.0f;
	}

	inline float Sample(float x, float y, const Settings& settings)
	{
		const SimplexNoise simplex(0.1f / settings.scale, 0.5f, settings.lacunarity, settings.persistance);
		return Sample(x, y, settings, simplex);
	}
};