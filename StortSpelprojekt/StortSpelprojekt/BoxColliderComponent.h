#pragma once
#include "Component.h"
#include "Collider.h"

namespace dx = DirectX;

class BoxColliderComponent : public Component, public Collider
{
public:

	BoxColliderComponent(dx::XMFLOAT3 extends, dx::XMFLOAT3 position);
	virtual~BoxColliderComponent() {};

	virtual void Initialize() override;
	dx::XMFLOAT3 GetExtends() const { return this->extends; }

private:
	dx::XMFLOAT3 extends;
};