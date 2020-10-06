#include "SphereColliderComponent.h"

SphereColliderComponent::SphereColliderComponent(float radius, dx::XMFLOAT3 position)
	:radius(radius), position(position)
{
}

void SphereColliderComponent::Initialize()
{
	shape = new btSphereShape(radius);

	btVector3 btPosition(position.x, position.y, position.z);
	transform.setIdentity();
	transform.setOrigin(btPosition);
}
