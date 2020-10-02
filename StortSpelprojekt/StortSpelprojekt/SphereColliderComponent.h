#pragma once
#include "Component.h"
#include "Collider.h"

class SphereColliderComponent : public Component, public Collider
{
public:
	SphereColliderComponent(float radius, float rX, float rY, float rZ, float rW, float pX, float pY, float pZ);
	virtual~SphereColliderComponent() {};

	virtual void Initialize() override;
	// Inherited via Collider
	virtual btCollisionShape* GetCollisionShape() const override;
	virtual btTransform GetTransform() const override;

private:

	float radius, rX, rY, rZ, rW, pX, pY, pZ;
	btCollisionShape* shape;

	btTransform trans;

	btQuaternion rotation;
	btVector3 position;
};
