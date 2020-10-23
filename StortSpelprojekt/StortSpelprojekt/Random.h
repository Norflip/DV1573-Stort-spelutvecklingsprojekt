#pragma once
#include <random>
#include <chrono>
#include "Math.h"

namespace Random
{
	static std::default_random_engine m_rngEngine (time(NULL));
	static std::uniform_real_distribution<float> m_distribution(0.0f, 1.0f);
	static bool m_seeded;
	
	inline void SetSeed(int seed)
	{
		m_rngEngine.seed(seed);
		m_seeded = true;
	}

	inline float Value()
	{
		return m_distribution(m_rngEngine);
	}

	inline void Value2D(float& x, float& y)
	{
		x = Value();
		y = Value();
	}


	inline float Range(float min, float max)
	{
		float range = (max - min);
		return min + range * Value();
	}

	inline int Range(int min, int max)
	{
		float range = (max - min);
		return min + range * Value();
	}



	inline float RadAngle()
	{
		return Value() * Math::PI * 2;
	}

	inline int GenerateSeed()
	{
		return time(NULL);
	}

	inline void PointOnDisc(float radius, float& x, float& y)
	{
		float angle = RadAngle();
		float offset = Value() * radius;
		x = cosf(angle) * offset;
		y = sinf(angle) * offset;
	}
}
