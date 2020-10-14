#include "CapsuleColliderComponent.h"
#include "Physics.h"

CapsuleColliderComponent::CapsuleColliderComponent(float radius, float height, dx::XMFLOAT3 position)
	: radius(radius), height(height), Collider(position)
{
}

void CapsuleColliderComponent::Initialize()
{
	rp::PhysicsCommon& common = Physics::Instance().GetCommon();
	shape = common.createCapsuleShape(radius, height);
}