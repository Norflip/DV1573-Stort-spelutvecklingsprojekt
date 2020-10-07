#include "ChunkCollider.h"
#include "Chunk.h"
#include "Object.h"

ChunkCollider::ChunkCollider(Chunk* chunk) : chunk(chunk)
{
}

ChunkCollider::~ChunkCollider()
{
}

void ChunkCollider::Initialize()
{
	transform.setIdentity();
	transform.setOrigin(btVector3(0, 0, 0));

	Chunk* chunk = GetOwner()->GetComponent<Chunk>();
	const int gridSize = static_cast<int>(CHUNK_SIZE) + 1;
	float* gridData = chunk->GetHeightMap();

	shape = new btHeightfieldTerrainShape(gridSize, gridSize, static_cast<void*>(gridData), 1, -TERRAIN_SCALE, TERRAIN_SCALE, 2, PHY_FLOAT, false);
}
