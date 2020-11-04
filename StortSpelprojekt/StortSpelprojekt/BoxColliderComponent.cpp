#include "stdafx.h"
#include "BoxColliderComponent.h"
#include "Physics.h"

BoxColliderComponent::BoxColliderComponent(std::vector<dx::XMFLOAT3> extends, std::vector<dx::XMFLOAT3> positions)
    :extends(extends), Collider(positions)
{
    assert(extends.size() == positions.size());
}

BoxColliderComponent::BoxColliderComponent(dx::XMFLOAT3 extends, dx::XMFLOAT3 position)
    : Collider(position)
{
    this->extends = std::vector<dx::XMFLOAT3>();
    this->extends.push_back(extends);
}

void BoxColliderComponent::InitializeCollider(Physics* physics)
{
    rp::PhysicsCommon& common = physics->GetCommon();

    for (size_t i = 0; i < colliderInformations.size(); i++)
    {
        dx::XMFLOAT3 extend = extends[i];
        colliderInformations[i].shape = common.createBoxShape(rp::Vector3(extend.x, extend.y, extend.z));
    }
}