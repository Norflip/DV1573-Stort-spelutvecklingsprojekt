#include "Collider.h"
#include "Physics.h"

Collider::Collider(dx::XMFLOAT3 position) : shape(nullptr)
{
    this->transform = rp::Transform(rp::Vector3(position.x, position.y, position.z), rp::Quaternion::identity());
}

void Collider::SetRotation(dx::XMVECTOR quaternion)
{
    dx::XMFLOAT4 quaternionValues;
    dx::XMStoreFloat4(&quaternionValues, quaternion);
    rp::Quaternion rpRotation(rotation.x, rotation.y, rotation.z, rotation.w);
    transform.setOrientation(rpRotation);
}