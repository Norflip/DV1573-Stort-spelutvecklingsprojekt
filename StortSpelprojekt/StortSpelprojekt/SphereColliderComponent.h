#pragma once
#include "Component.h"
#include "Collider.h"

namespace dx = DirectX;

class SphereColliderComponent : public Component, public Collider
{
public:
	SphereColliderComponent(float radius, dx::XMFLOAT3 position);
	virtual~SphereColliderComponent() {};
	virtual void Initialize() override;

private:
	float radius;
};
