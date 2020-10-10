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
	const int m_upAxis = 1;

	float* gridData = chunk->GetHeightMap();	





	btHeightfieldTerrainShape* heightShape = new btHeightfieldTerrainShape(gridSize, gridSize, static_cast<void*>(gridData), 1, -TERRAIN_SCALE, TERRAIN_SCALE, m_upAxis, PHY_FLOAT, false);

	//if (m_upAxis == 2)
	//	heightShape->setFlipTriangleWinding(true);

	shape = heightShape;
}
