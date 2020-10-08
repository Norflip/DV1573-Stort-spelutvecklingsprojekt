#pragma once
#include <DirectXMath.h>
namespace dx = DirectX;

#include "Component.h"
#include "Math.h"

constexpr int CHUNK_SIZE = 32;
constexpr float TERRAIN_SCALE = 10.0f;

enum class ChunkType
{
	Path,
	Start,
	Goal,
	Event
};


class Chunk : public Component
{
public:
	Chunk(dx::XMINT2 index, ChunkType type);
	virtual ~Chunk();

	float SampleHeight(float x, float z);

	void SetHeightMap(float* heightmap) { this->heightMap = heightmap; }
	dx::XMINT2 GetIndex() const { return this->index; }
	ChunkType GetType() const { return this->type; }

	static dx::XMVECTOR IndexToWorld(const dx::XMINT2& index, float y);
	static dx::XMFLOAT2 IndexToXZ(const dx::XMINT2& index);

private:
	dx::XMINT2 index;
	ChunkType type;
	float* heightMap;
};
