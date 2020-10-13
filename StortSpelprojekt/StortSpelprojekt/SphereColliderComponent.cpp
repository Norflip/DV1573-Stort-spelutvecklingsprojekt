#include "SphereColliderComponent.h"
#include "Physics.h"

SphereColliderComponent::SphereColliderComponent(float radius, dx::XMFLOAT3 position)
	:radius(radius), position(position)
{
}

void SphereColliderComponent::Initialize()
{
	this->transform = rp::Transform(rp::Vector3(position.x, position.y, position.z), rp::Quaternion::identity());

	rp::PhysicsCommon& common = Physics::Instance().GetCommon();
	shape = common.createSphereShape(radius);
}
