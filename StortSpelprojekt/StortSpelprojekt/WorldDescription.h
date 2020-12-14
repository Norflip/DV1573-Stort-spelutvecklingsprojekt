#pragma once
#include "Noise.h"

struct WorldDescription
{
	int maxSteps;
	int directionalSteps;

	int minEnviromentProps;
	int maxEnviromentProps;

	int leftIndex;
	int rightIndex;

	std::map<std::string, int> queueModifier;
	Noise::Settings noiseSettings;

	WorldDescription() 
	{
	
	
	}


	WorldDescription(int noiseSeed)
	{
		this->noiseSettings = Noise::Settings (noiseSeed);
		this->noiseSettings.octaves = 5;
		this->noiseSettings.persistance = 0.5f;
		this->noiseSettings.lacunarity = 1.4f;
		this->noiseSettings.scale = 4.f;
	}
};