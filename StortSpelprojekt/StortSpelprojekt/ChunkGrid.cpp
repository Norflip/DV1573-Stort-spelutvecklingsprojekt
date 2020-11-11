#include "stdafx.h"
#include "ChunkGrid.h"

void ChunkGrid::Generate(int maxSteps, int padding, int directionalSteps)
{
	Clear();
	std::vector<dx::XMINT2> points;
	std::vector<dx::XMINT2> pathIndexes;

	dx::XMINT2 current(0, 0);
	dx::XMINT2 direction(0, 1);

	int steps = maxSteps;
	while (steps > 0)
	{
		for (size_t i = 0; i < UICAST(directionalSteps); i++)
		{
			current.x += direction.x;
			current.y += direction.y;

			pathIndexes.push_back(dx::XMINT2(current));
			points.push_back(dx::XMINT2(current));
			steps--;
		}
		UpdateDirection(direction);
	}

	size_t size = points.size();

	AddChunksFromPath(points, pathIndexes, chunks);

	dx::XMINT2 index;
	AddPrePostChunks(points[0], dx::XMINT2(0, -1), NON_WALK_STEPS, points, index, chunks);
	pathIndexes.insert(pathIndexes.begin(), index);

	//AddPrePostChunks(points[size - 1], dx::XMINT2(0,1), NON_WALK_STEPS, points, index, chunks);
	//pathIndexes.push_back(index);

	AddPadding(padding, points, chunks);

	FindMinMax(chunks);
	path = Path(pathIndexes);
}

void ChunkGrid::Clear()
{
	chunks.clear();
	path.Clear();
}

void ChunkGrid::AddChunksFromPath(std::vector<dx::XMINT2>& points, std::vector<dx::XMINT2>& path, std::unordered_map<int, ChunkIndexInfo>& chunks)
{
	size_t size = points.size();
	for (size_t i = 0; i < size; i++)
	{
		int index = HASH2D_I(points[i].x, points[i].y);
		ChunkIndexInfo info;
		info.index = points[i];
		info.type = ChunkType::PATH;

		if (i == 0)
			info.type = ChunkType::START;
		else if (i == size - 1)
			info.type = ChunkType::END;

		chunks.insert({ index, info });
	}
}

void ChunkGrid::AddPrePostChunks(const dx::XMINT2& start, const dx::XMINT2& direction, const int& steps, std::vector<dx::XMINT2>& points, dx::XMINT2& last, std::unordered_map<int, ChunkIndexInfo>& chunks)
{
	dx::XMINT2 current = start;

	for (size_t i = 0; i < UICAST(steps); i++)
	{
		current.x += direction.x;
		current.y += direction.y;
		int index = HASH2D_I(current.x, current.y);
		auto find = chunks.find(index);

		if (find == chunks.end())
		{
			ChunkIndexInfo info;
			info.index = current;
			info.type = ChunkType::NON_PATH;

			points.push_back(dx::XMINT2(current));
			chunks.insert({ index, info });
		}
	}

	last = dx::XMINT2(current);
}

void ChunkGrid::AddPadding(int radius, std::vector<dx::XMINT2>& points, std::unordered_map<int, ChunkIndexInfo>& chunks)
{
	int currentX, currentY;
	size_t size = points.size();

	for (size_t i = 0; i < size; i++)
	{
		for (int y = -radius; y <= radius; y++)
		{
			for (int x = -radius; x <= radius; x++)
			{
				if (x == 0 && y == 0)
					continue;

				currentX = points[i].x + x;
				currentY = points[i].y + y;
				int index = HASH2D_I(currentX, currentY);
				
				auto find = chunks.find(index);
				if (find == chunks.end())
				{
					ChunkIndexInfo info;
					info.index = dx::XMINT2(currentX, currentY);
					info.type = ChunkType::TERRAIN;

					chunks.insert({ index, info });
				}
			}
		}
	}
}

void ChunkGrid::UpdateDirection(dx::XMINT2& direction)
{
	if (direction.y == 0)
	{
		float value = Random::Value();

		// 50% att den går rakt vertikalt
		if (value < 0.5f)
		{
			direction.x = 0;
			direction.y = 1;
		}
		// 50% att den går helt åt sidan
		else if (value >= 0.5f)
		{
			direction.y = 1;
		}
	}
	else
	{
		// 50% om den byter riktning i x
		direction.x = (Random::Value() < 0.5f)? -1 : 1;

		// 50% om den börjar gå horizontal
		if (Random::Value() < 0.5f)
		{
			direction.y = 0;
		}
	}
}

void ChunkGrid::FindMinMax(std::unordered_map<int, ChunkIndexInfo>& chunks)
{
	bool first = true;
	dx::XMINT2 index;

	for (auto i : chunks)
	{
		index = i.second.index;
		if (first)
		{
			minIndex = maxIndex = index;
			first = false;
		}
		else
		{
			minIndex.x = std::min(minIndex.x, index.x);
			minIndex.y = std::min(minIndex.y, index.y);
			maxIndex.x = std::max(maxIndex.x, index.x);
			maxIndex.y = std::max(maxIndex.y, index.y);
		}
	}
}
