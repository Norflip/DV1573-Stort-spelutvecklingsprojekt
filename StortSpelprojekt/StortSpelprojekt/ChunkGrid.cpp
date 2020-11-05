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

	AddPrePostChunks(points[size - 1], dx::XMINT2(0,1), NON_WALK_STEPS, points, index, chunks);
	pathIndexes.push_back(index);

	AddPadding(padding, points, chunks);

	FindMinMax(chunks);
	path = Path(pathIndexes);
}

void ChunkGrid::Clear()
{
	chunks.clear();
	path.Clear();
}

void ChunkGrid::AddChunksFromPath(std::vector<dx::XMINT2>& points, std::vector<dx::XMINT2>& path, std::unordered_map<int, ChunkInfo>& chunks)
{
	size_t size = points.size();
	for (size_t i = 0; i < size; i++)
	{
		int index = HASH2D_I(points[i].x, points[i].y);
		ChunkInfo chunkInfo = std::make_pair(points[i], ChunkType::PATH);
		
		if (i == 0)
			chunkInfo.second = ChunkType::START;
		else if (i == size - 1)
			chunkInfo.second = ChunkType::END;

		chunks.insert({ index, chunkInfo });
	}
}

void ChunkGrid::AddPrePostChunks(const dx::XMINT2& start, const dx::XMINT2& direction, const int& steps, std::vector<dx::XMINT2>& points, dx::XMINT2& last, std::unordered_map<int, ChunkInfo>& chunks)
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
			points.push_back(dx::XMINT2(current));
			chunks.insert({ index, std::make_pair(current, ChunkType::NON_PATH) });
		}
	}

	last = dx::XMINT2(current);
}

void ChunkGrid::AddPadding(int radius, std::vector<dx::XMINT2>& points, std::unordered_map<int, ChunkInfo>& chunks)
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
					chunks.insert({ index, std::make_pair(dx::XMINT2(currentX, currentY), ChunkType::TERRAIN) });
				}
			}
		}
	}
}

void ChunkGrid::UpdateDirection(dx::XMINT2& direction)
{
	float value = Random::Value();
	if (direction.y == 0)
	{
		if (value < 0.65f)
		{
			direction.x = 0;
			direction.y = 1;
		}
	}
	else
	{
		direction.x = (value < 0.5f)? -1 : 1;
		direction.y = 0;
	}
}

void ChunkGrid::FindMinMax(std::unordered_map<int, ChunkInfo>& chunks)
{
	bool first = true;
	dx::XMINT2 index;

	for (auto i : chunks)
	{
		index = i.second.first;
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
