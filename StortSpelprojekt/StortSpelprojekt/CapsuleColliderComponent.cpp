#include "stdafx.h"
#include "CapsuleColliderComponent.h"
#include "Physics.h"

CapsuleColliderComponent::CapsuleColliderComponent(float radius, float height, dx::XMFLOAT3 position)
	: Collider(position)
{
	this->heights.push_back(height);
	this->radius.push_back(radius);
}

CapsuleColliderComponent::CapsuleColliderComponent(std::vector<float> radius, std::vector<float> heights, std::vector<dx::XMFLOAT3> positions)
	: Collider(positions)
{
}

void CapsuleColliderComponent::InitializeCollider(Physics* physics)
{
	rp::PhysicsCommon& common = physics->GetCommon();

	for (size_t i = 0; i < colliderInformations.size(); i++)
		colliderInformations[i].shape = common.createCapsuleShape(radius[i], heights[i]);
}
