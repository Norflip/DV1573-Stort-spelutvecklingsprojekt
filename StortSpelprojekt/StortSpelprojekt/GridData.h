 #pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "Chunk.h"
#include "Random.h"
#include "Path2.h"

class GridData
{
	const int NON_WALK_STEPS = 2;
public:
	typedef std::pair<dx::XMINT2, ChunkType> ChunkInfo;

	GridData() {};
	virtual ~GridData() {};

	void Generate(int maxSteps, int padding, int directionalSteps = 1);
	void Clear();

	std::unordered_map<int, ChunkInfo>& GetChunks() { return this->chunks; }
	Path2& GetPath() { return this->path; }
	dx::XMINT2 GetMinIndex() const { return this->minIndex; }
	dx::XMINT2 GetMaxIndex() const { return this->maxIndex; }

private:
	void AddChunksFromPath(std::vector<dx::XMINT2>& points, std::vector<dx::XMINT2>& path, std::unordered_map<int, ChunkInfo>& chunks);
	void AddPrePostChunks(const dx::XMINT2& start, const dx::XMINT2& direction, const int& steps, std::vector<dx::XMINT2>& points, dx::XMINT2& last, std::unordered_map<int, ChunkInfo>& chunks);
	void AddPadding(int radius, std::vector<dx::XMINT2>& points, std::unordered_map<int, ChunkInfo>& chunks);

	void UpdateDirection(dx::XMINT2& direction);
	void FindMinMax(std::unordered_map<int, ChunkInfo>& chunks);

private:
	std::unordered_map<int, ChunkInfo> chunks;
	Path2 path;
	dx::XMINT2 minIndex, maxIndex;
};