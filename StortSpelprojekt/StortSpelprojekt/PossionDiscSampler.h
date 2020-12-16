#pragma once
#include "Random.h"
namespace dx = DirectX;

namespace PossionDiscSampler
{
	inline bool m_IsValid(dx::XMFLOAT2 point, float width, float height, float cellSize, float radius, std::vector<dx::XMFLOAT2>& points, int* grid, int cols, int rows)
	{
		if (point.x >= 0 && point.x < width && point.y >= 0 && point.y < height)
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
					size_t index = UICAST(grid[x * cols + y] - 1);
					if (index >= 0 && index < points.size())
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

	inline std::vector<dx::XMFLOAT2> Sample(float radius, float width, float height, int rejectionCount = 5)
	{
		std::vector<dx::XMFLOAT2> points;
		std::vector<dx::XMFLOAT2> spawnPoints;

		const float cellSize = radius / sqrtf(2.0f);
		const int cols = static_cast<int>(ceilf(width / cellSize));
		const int rows = static_cast<int>(ceilf(height / cellSize));
		int* grid = new int[cols * rows];

		for (size_t i = 0; i < UICAST(cols * rows); i++)
			grid[i] = 0;

		spawnPoints.push_back(dx::XMFLOAT2(width / 2.0f, height / 2.0f));

		while (spawnPoints.size() > 0 && points.size() < 500)
		{
			// RANDOM
			int index = Random::Range(0, spawnPoints.size() - 1);

			dx::XMFLOAT2 center = spawnPoints[index];
			bool accepted = false;

			for (size_t i = 0; i < UICAST(rejectionCount); i++)
			{
				// random.value
				float angle = Random::RadAngle();
				float offset = Random::Range(radius, radius * 2.0f);// (distribution(rngEngine) + 1.0f)* radius; // random mellan radius och radius * 2


				dx::XMFLOAT2 direction(sinf(angle), cosf(angle));
				dx::XMFLOAT2 point(center.x + direction.x * offset, center.y + direction.y * offset);

				if (m_IsValid(point, width, height, cellSize, radius, points, grid, cols, rows))
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
		delete[] grid;
		return points;
	}
}
