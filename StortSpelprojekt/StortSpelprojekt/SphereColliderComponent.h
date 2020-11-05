#pragma once
#include "Component.h"
#include "Collider.h"

class SphereColliderComponent : public Component, public Collider
{
public:
	SphereColliderComponent(float radius, dx::XMFLOAT3 position);
	SphereColliderComponent(std::vector<float> radius, std::vector<dx::XMFLOAT3> position);
	virtual ~SphereColliderComponent() {};
	virtual void Initialize() override;

private:
	std::vector<float> radius;
};
