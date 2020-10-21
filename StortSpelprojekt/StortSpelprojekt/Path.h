#pragma once
#include <random>
#include <DirectXMath.h>
namespace dx = DirectX;
#include "Math.h"

#define Wrap(index, size) (((index) % (size)) + (size)) % (size)

class Path
{
public:
	Path(int seed);
	virtual ~Path();

	void Walk(int indexSteps, int directionalSteps = 1);
	void Clear();

	void Reseed(int seed);

	float ClosestDistance(const dx::XMFLOAT2& position) const;
	size_t IndexCount() const { return this->indexes.size(); }

	std::vector<dx::XMFLOAT2>& GetPoints() { return this->points; }
	std::vector<dx::XMINT2>& GetIndexes() { return this->indexes; }

private:
	void CalculateIndexes(int steps, int directionalSteps);
	void CalculatePoints();
	void UpdateDirection(dx::XMINT2& direction);
	std::vector<dx::XMFLOAT2> SmoothPoints(std::vector<dx::XMFLOAT2>& points, float smoothness, float cornerRadius);

private:
	std::vector<dx::XMINT2> indexes;
	std::vector<dx::XMFLOAT2> points;

	std::default_random_engine rngEngine;
	std::uniform_real_distribution<float> distribution;

};