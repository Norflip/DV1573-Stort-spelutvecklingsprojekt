#pragma once
#include "Noise.h"

struct SegmentDescription
{
	int maxSteps;
	int directionalSteps;
	std::map<std::string, int> queueModifier;
	Noise::Settings noiseSettings;

	SegmentDescription(int noiseSeed, int maxSteps, int directionalSteps)
	{
		this->maxSteps = maxSteps;
		this->directionalSteps = directionalSteps;

		this->noiseSettings = Noise::Settings (noiseSeed);
		this->noiseSettings.octaves = 5;
		this->noiseSettings.persistance = 0.5f;
		this->noiseSettings.lacunarity = 1.4f;
		this->noiseSettings.scale = 4.f;
	}
};