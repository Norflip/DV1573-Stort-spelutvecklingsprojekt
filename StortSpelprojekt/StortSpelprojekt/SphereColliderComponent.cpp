#include "stdafx.h"
#include "SphereColliderComponent.h"
#include "Physics.h"
#include "Engine.h"

SphereColliderComponent::SphereColliderComponent(float radius, dx::XMFLOAT3 position)
	: Collider(position) 
{
	this->radius.push_back(radius);
}

SphereColliderComponent::SphereColliderComponent(std::vector<float> radius, std::vector<dx::XMFLOAT3> positions) 
	: radius(radius), Collider(positions) {}

SphereColliderComponent::~SphereColliderComponent()
{
	rp::PhysicsCommon& common = Engine::Instance->GetPhysics()->GetCommon();

	for (size_t i = 0; i < colliderInformations.size(); i++)
	{
		common.destroySphereShape(static_cast<rp::SphereShape*>(colliderInformations[i].shape));
	}
}

void SphereColliderComponent::InitializeCollider(Physics* physics)
{
	rp::PhysicsCommon& common = physics->GetCommon();
	for (size_t i = 0; i < colliderInformations.size(); i++)
		colliderInformations[i].shape = common.createSphereShape(radius[i]);
}
