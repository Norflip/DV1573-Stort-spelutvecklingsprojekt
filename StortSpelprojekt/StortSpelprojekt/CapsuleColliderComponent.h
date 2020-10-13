#pragma once
#include "Component.h"
#include "Collider.h"

namespace dx = DirectX;

class CapsuleColliderComponent : public Component, public Collider
{
public:

	CapsuleColliderComponent(float radius, float height, dx::XMFLOAT3 position);
	virtual~CapsuleColliderComponent() {};
	virtual void Initialize() override;

private:
	float radius, height;
	dx::XMFLOAT3 position;
};
