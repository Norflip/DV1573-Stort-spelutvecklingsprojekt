#pragma once
#include "Component.h"
#include "Collider.h"

class SphereColliderComponent : public Component, public Collider
{
public:
	SphereColliderComponent(float radius);
	virtual~SphereColliderComponent() {};

	virtual void Initialize() override;
	// Inherited via Collider
	virtual btCollisionShape* GetCollisionShape() const override;

private:

	float radius;
	btCollisionShape* shape;
};
