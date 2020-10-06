#pragma once
#include "Component.h"
#include "Collider.h"
#include <vector>

namespace dx = DirectX;

class BoxColliderComponent : public Component, public Collider
{
public:

	BoxColliderComponent(dx::XMFLOAT3 extends, dx::XMFLOAT3 position, dx::XMFLOAT4 quaterionRotation);
	virtual~BoxColliderComponent() {};

	virtual void Initialize() override;
	dx::XMFLOAT3 GetExtends() const { return this->extends; }

private:
	
	dx::XMFLOAT3 position;
	dx::XMFLOAT4 rotation;
	dx::XMFLOAT3 extends;
};