#pragma once
#include "Component.h"
#include "Collider.h"
#include "Bulletphysics/btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"

class ChunkCollider : public Component, public Collider
{
	ChunkCollider();
	virtual~ChunkCollider();

	virtual void Initialize() override;
};