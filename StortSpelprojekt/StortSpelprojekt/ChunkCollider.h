#pragma once
#include "Component.h"
#include "Collider.h"
#include "Bulletphysics/btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "Chunk.h"

class ChunkCollider : public Component, public Collider
{
public:
	ChunkCollider(Chunk* chunk);
	virtual~ChunkCollider();

	virtual void Initialize() override;

private:
	Chunk* chunk;
};