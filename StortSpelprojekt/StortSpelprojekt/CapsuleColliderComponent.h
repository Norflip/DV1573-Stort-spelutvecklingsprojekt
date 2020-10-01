#pragma once
#include "Component.h"
#include "Collider.h"

class CapsuleColliderComponent : public Component, public Collider
{
public:

	CapsuleColliderComponent(float radius, float height);
	virtual~CapsuleColliderComponent() {};

	virtual void Initialize() override;
	// Inherited via Collider
	virtual btCollisionShape* GetCollisionShape() const override;

private:
	float radius, height;
	btCollisionShape* shape;
};
