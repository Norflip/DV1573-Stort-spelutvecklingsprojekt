#include "CapsuleColliderComponent.h"
#include "Physics.h"

CapsuleColliderComponent::CapsuleColliderComponent(float radius, float height, dx::XMFLOAT3 position)
	: radius(radius), height(height), position(position)
{
}

void CapsuleColliderComponent::Initialize()
{
	this->transform = rp::Transform(rp::Vector3(position.x, position.y, position.z), rp::Quaternion::identity());

	rp::PhysicsCommon& common = Physics::Instance().GetCommon();
	shape = common.createCapsuleShape(radius, height);
}