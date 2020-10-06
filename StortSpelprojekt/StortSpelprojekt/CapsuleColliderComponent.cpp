#include "CapsuleColliderComponent.h"


CapsuleColliderComponent::CapsuleColliderComponent(float radius, float height, dx::XMFLOAT3 position)
	: radius(radius), height(height), position(position)
{
}

void CapsuleColliderComponent::Initialize()
{
	shape = new btCapsuleShape(radius, height);

	btVector3 btPosition(position.x, position.y, position.z);
	transform.setIdentity();
	transform.setOrigin(btPosition);
}