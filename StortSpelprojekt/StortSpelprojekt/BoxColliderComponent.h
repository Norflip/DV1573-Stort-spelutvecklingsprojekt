#pragma once
#include "Component.h"
#include "Collider.h"

namespace dx = DirectX;

class BoxColliderComponent : public Component, public Collider
{
public:
	BoxColliderComponent(std::vector<dx::XMFLOAT3> extends, std::vector<dx::XMFLOAT3> positions);
	BoxColliderComponent(dx::XMFLOAT3 extends, dx::XMFLOAT3 position);
	virtual~BoxColliderComponent() {};

	virtual void Initialize() override;
	dx::XMFLOAT3 GetExtends(size_t index = 0) const { return this->extends[0]; }

private:
	std::vector<dx::XMFLOAT3> extends;
	std::vector<dx::XMFLOAT3> positions;
};