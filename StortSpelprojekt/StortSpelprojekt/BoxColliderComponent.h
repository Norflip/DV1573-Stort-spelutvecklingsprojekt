#pragma once
#include "Component.h"
#include "Collider.h"
#include <vector>

class BoxColliderComponent : public Component, public Collider
{
public:

	BoxColliderComponent(float x, float y, float z, float rX, float rY, float rZ, float rW, float pX, float pY, float pZ);
	virtual~BoxColliderComponent() {};

	virtual void Initialize() override;
	// Inherited via Collider
	virtual btCollisionShape* GetCollisionShape()const override;
	virtual btTransform GetTransform() const override;

private:

	float x, y, z, rX, rY, rZ, rW, pX, pY, pZ;
	btCollisionShape* shape;

	btTransform trans;

	btQuaternion rotation;
	btVector3 position;
	
};