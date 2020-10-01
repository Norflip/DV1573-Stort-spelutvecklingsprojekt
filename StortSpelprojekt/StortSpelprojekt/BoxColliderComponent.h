#pragma once
#include "Component.h"
#include "Collider.h"

class BoxColliderComponent : public Component, public Collider
{
public:

	BoxColliderComponent(float x, float y, float z);
	virtual~BoxColliderComponent() {};

	virtual void Initialize() override;
	// Inherited via Collider
	virtual btCollisionShape* GetCollisionShape() const override;

private:

	float x, y, z;
	btCollisionShape* shape;
};