#pragma once
#include "Component.h"
#include "Collider.h"

namespace dx = DirectX;

class CapsuleColliderComponent : public Component, public Collider
{
public:
	CapsuleColliderComponent(float radius, float height, dx::XMFLOAT3 position);
	CapsuleColliderComponent(std::vector<float> radius, std::vector<float> heights, std::vector<dx::XMFLOAT3> positions);

	virtual~CapsuleColliderComponent();

	void DeleteShapes() override;
	void InitializeCollider(Physics* physics) override;

private:
	std::vector<float> radius;
	std::vector<float> heights;
};
