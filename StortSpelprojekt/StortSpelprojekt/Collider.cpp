#include "stdafx.h"
#include "Collider.h"
#include "Physics.h"

Collider::Collider(std::vector<dx::XMFLOAT3> positions)
{
    assert(positions.size() > 0);

    size_t size = positions.size();
    this->colliderInformations.resize(positions.size());

    for (size_t i = 0; i < positions.size(); i++)
    {
        colliderInformations[i].position = positions[i];
        dx::XMStoreFloat4(&colliderInformations[i].rotation, dx::XMQuaternionIdentity());
        rp::Vector3 rpPos(positions[i].x, positions[i].y, positions[i].z);
        colliderInformations[i].transform = rp::Transform(rpPos, rp::Quaternion::identity());
        colliderInformations[i].shape = nullptr;
    }
}

Collider::Collider(dx::XMFLOAT3 position)
{
    this->colliderInformations.resize(1);
    colliderInformations[0].position = position;
    dx::XMStoreFloat4(&colliderInformations[0].rotation, dx::XMQuaternionIdentity());
    
    rp::Vector3 rpPos (position.x, position.y, position.z);
    colliderInformations[0].transform = rp::Transform(rpPos, rp::Quaternion::identity());
    colliderInformations[0].shape = nullptr;
}

rp::CollisionShape* Collider::GetCollisionShape(size_t index) const
{
    assert(index >= 0 && index < colliderInformations.size()); 
    return this->colliderInformations[index].shape;
}

rp::Transform Collider::GetTransform(size_t index) const
{
    assert(index >= 0 && index < colliderInformations.size());
    return this->colliderInformations[index].transform;
}

void Collider::SetRotation(size_t index, dx::XMVECTOR quaternion)
{
    assert(index >= 0 && index < colliderInformations.size());
    dx::XMStoreFloat4(&colliderInformations[index].rotation, quaternion);
}