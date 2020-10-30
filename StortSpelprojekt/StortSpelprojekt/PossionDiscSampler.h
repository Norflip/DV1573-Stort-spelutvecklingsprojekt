#pragma once


namespace dx = DirectX;

typedef std::vector<dx::XMFLOAT2> Points;

class PossionDiscSampler
{
public:
	PossionDiscSampler(int seed = 0);
	virtual ~PossionDiscSampler();

	Points GeneratePoints(float radius, dx::XMFLOAT2 regionSize, bool compareToWorld, int rejectionCount = 5);
	void ResetWorld ();
	void Reseed(int seed);

private:
	bool IsValid(dx::XMFLOAT2 point, dx::XMFLOAT2 regionSize, float cellSize, float radius, Points& points, int* grid, int cols, int rows);

	std::default_random_engine rngEngine;
	std::uniform_real_distribution<float> distribution;
};