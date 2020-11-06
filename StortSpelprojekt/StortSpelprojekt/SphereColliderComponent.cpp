#include "stdafx.h"
#include "SphereColliderComponent.h"
#include "Physics.h"

SphereColliderComponent::SphereColliderComponent(float radius, dx::XMFLOAT3 position)
	: Collider(position) 
{
	this->radius.push_back(radius);
}

SphereColliderComponent::SphereColliderComponent(std::vector<float> radius, std::vector<dx::XMFLOAT3> positions) 
	: radius(radius), Collider(positions) {}

void SphereColliderComponent::InitializeCollider(Physics* physics)
{
	rp::PhysicsCommon& common = physics->GetCommon();
	for (size_t i = 0; i < colliderInformations.size(); i++)
		colliderInformations[i].shape = common.createSphereShape(radius[i]);
}
