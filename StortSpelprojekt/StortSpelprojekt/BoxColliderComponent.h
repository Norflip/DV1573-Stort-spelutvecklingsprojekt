#pragma once
#include "Component.h"
#include "Collider.h"

namespace dx = DirectX;
class Physics;

class BoxColliderComponent : public Component, public Collider
{
public:
	BoxColliderComponent(std::vector<dx::XMFLOAT3> extends, std::vector<dx::XMFLOAT3> positions);
	BoxColliderComponent(dx::XMFLOAT3 extends, dx::XMFLOAT3 position);
	virtual~BoxColliderComponent();

	void InitializeCollider (Physics* physics) override;
	dx::XMFLOAT3 GetExtends(size_t index = 0) const { return this->extends[0]; }

	void Update(const float& deltaTime) override;

private:
	std::vector<dx::XMFLOAT3> extends;
};