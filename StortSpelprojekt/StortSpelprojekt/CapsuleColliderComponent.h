#pragma once
#include "Component.h"
#include "Collider.h"

class CapsuleColliderComponent : public Component, public Collider
{
public:

	CapsuleColliderComponent(float radius, float height, float rX, float rY, float rZ, float rW, float pX, float pY, float pZ);
	virtual~CapsuleColliderComponent() {};

	virtual void Initialize() override;
	// Inherited via Collider
	virtual btCollisionShape* GetCollisionShape() const override;
	virtual btTransform GetTransform() const override;

private:
	float radius, height, rX, rY, rZ, rW, pX, pY, pZ;
	btCollisionShape* shape;

	btTransform trans;

	btQuaternion rotation;
	btVector3 position;
};
