#include "BoxColliderComponent.h"
#include "Physics.h"

BoxColliderComponent::BoxColliderComponent(dx::XMFLOAT3 extends, dx::XMFLOAT3 position, dx::XMFLOAT4 quaterionRotation)
    : extends(extends), position(position), rotation(quaterionRotation)
{

}

void BoxColliderComponent::Initialize()
{
    this->transform = rp::Transform(rp::Vector3(position.x, position.y, position.z), rp::Quaternion::identity());


    rp::PhysicsCommon& common = Physics::Instance().GetCommon();
    shape = common.createBoxShape(rp::Vector3(extends.x, extends.y, extends.z));

}