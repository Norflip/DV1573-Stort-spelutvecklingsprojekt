#pragma once
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include "ThirdParty\SimplexNoise.h"
#include "Chunk.h"
#include "Log.h"
#include "Random.h"

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

		Settings() {}
		Settings(int seed)
		{
			this->seed = seed;
			this->scale = 20.0f;
			this->octaves = 4;
			this->persistance = 0.0f;
			this->lacunarity = 0.0f;

			this->xOffset = Random::Range(-100000.f, 100000.f);
			this->yOffset = Random::Range(-100000.f, 100000.f);
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