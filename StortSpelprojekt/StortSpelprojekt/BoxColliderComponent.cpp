#include "stdafx.h"
#include "BoxColliderComponent.h"
#include "Physics.h"

BoxColliderComponent::BoxColliderComponent(dx::XMFLOAT3 extends, dx::XMFLOAT3 position)
    : extends(extends), Collider(position)
{

}

void BoxColliderComponent::Initialize()
{
    rp::PhysicsCommon& common = Physics::Instance().GetCommon();
    shape = common.createBoxShape(rp::Vector3(extends.x, extends.y, extends.z));
}