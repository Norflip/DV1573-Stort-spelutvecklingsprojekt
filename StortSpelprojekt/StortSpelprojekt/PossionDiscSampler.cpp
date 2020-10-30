#include "stdafx.h"
#include "PossionDiscSampler.h"

PossionDiscSampler::PossionDiscSampler(int seed) : distribution(0.0f, 1.0f)
{
	Reseed(seed);
}

PossionDiscSampler::~PossionDiscSampler()
{
}

Points PossionDiscSampler::GeneratePoints(float radius, dx::XMFLOAT2 regionSize, bool compareToWorld, int rejectionCount)
{
	Points points;
	Points spawnPoints;

	const float cellSize = radius / sqrtf(2.0f);
	const int cols = static_cast<int>(ceilf(regionSize.x / cellSize));
	const int rows = static_cast<int>(ceilf(regionSize.y / cellSize));
	int* grid = new int[cols * rows];

	for (size_t i = 0; i < cols * rows; i++)
		grid[i] = 0;

	spawnPoints.push_back(dx::XMFLOAT2(regionSize.x / 2.0f, regionSize.y / 2.0f));

	while (spawnPoints.size() > 0 && points.size() < 500)
	{
		// RANDOM
		int index = static_cast<int>(floorf((spawnPoints.size() - 1) * distribution(rngEngine)));

		dx::XMFLOAT2 center = spawnPoints[index];
		bool accepted = false;

		for (size_t i = 0; i < rejectionCount; i++)
		{
			// random.value
			float angle = distribution(rngEngine) * Math::PI * 2.0f;
			float offset = (distribution(rngEngine) + 1.0f) * radius; // random mellan radius och radius * 2


			dx::XMFLOAT2 direction(sinf(angle), cosf(angle));
			dx::XMFLOAT2 point(center.x + direction.x * offset, center.y + direction.y * offset);

			if (IsValid(point, regionSize, cellSize, radius, points, grid, cols, rows))
			{
				points.push_back(point);
				spawnPoints.push_back(point);

				accepted = true;

				int x = static_cast<int>(point.x / cellSize);
				int y = static_cast<int>(point.y / cellSize);
				grid[x * cols + y] = points.size();

				break;
			}
		}

		if (!accepted)
		{
			spawnPoints.erase(spawnPoints.begin() + index);
		}
	}

	return points;
}

void PossionDiscSampler::ResetWorld()
{
	//@TODO
}

void PossionDiscSampler::Reseed(int seed)
{
	rngEngine.seed(seed);
	distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);
}

bool PossionDiscSampler::IsValid(dx::XMFLOAT2 point, dx::XMFLOAT2 regionSize, float cellSize, float radius, Points& points, int* grid, int cols, int rows)
{
	if (point.x >= 0 && point.x < regionSize.x && point.y >= 0 && point.y < regionSize.y)
	{
		const float radiusSqr = radius * radius;
		int cellX = static_cast<int>(point.x / cellSize);
		int cellY = static_cast<int>(point.y / cellSize);

		int xmin = std::max(0, cellX - 2);
		int xmax = std::min(cellX + 2, cols - 1);
		int ymin = std::max(0, cellY - 2);
		int ymax = std::min(cellY + 2, rows - 1);

		for (int x = xmin; x <= xmax; x++)
		{
			for (int y = ymin; y <= ymax; y++)
			{
				int index = grid[x * cols + y] - 1;

				if (index != -1)
				{
					dx::XMFLOAT2 found = points[index];
					dx::XMFLOAT2 differance(point.x - found.x, point.y - found.y);

					float distanceSqr = differance.x * differance.x + differance.y * differance.y;
					if (distanceSqr < radiusSqr)
					{
						return false;
					}
				}
			}
		}

		return true;
	}

	return false;
}
