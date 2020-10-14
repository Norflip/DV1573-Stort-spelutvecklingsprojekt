#include "SphereColliderComponent.h"
#include "Physics.h"

SphereColliderComponent::SphereColliderComponent(float radius, dx::XMFLOAT3 position)
	:radius(radius), Collider(position)
{
}

void SphereColliderComponent::Initialize()
{
	rp::PhysicsCommon& common = Physics::Instance().GetCommon();
	shape = common.createSphereShape(radius);
}
