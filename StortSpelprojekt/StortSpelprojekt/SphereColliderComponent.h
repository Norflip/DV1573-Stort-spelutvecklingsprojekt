#pragma once
#include "Component.h"
#include "Collider.h"
class Physics;

class SphereColliderComponent : public Component, public Collider
{
public:
	SphereColliderComponent(float radius, dx::XMFLOAT3 position);
	SphereColliderComponent(std::vector<float> radius, std::vector<dx::XMFLOAT3> position);
	virtual ~SphereColliderComponent();

	void InitializeCollider(Physics* physics) override;
	void Update(const float& deltaTime) override;
private:
	std::vector<float> radius;
};
