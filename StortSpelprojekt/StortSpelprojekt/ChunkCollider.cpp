#include "ChunkCollider.h"
#include "Chunk.h"

void ChunkCollider::Initialize()
{
	int gridSize = CHUNK_SIZE;
	float* gridData = new float[64*64];

	btScalar height = 10.0f;
	const void* data = static_cast<void*>(gridData);

	shape = new btHeightfieldTerrainShape(64, 64, data, 1, -height, height, 2, PHY_FLOAT, true);
}
