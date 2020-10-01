#include "CapsuleColliderComponent.h"

CapsuleColliderComponent::CapsuleColliderComponent(float radius, float height)
    :radius(radius), height(height)
{
}

void CapsuleColliderComponent::Initialize()
{
    shape = new btCapsuleShape(btScalar(radius), btScalar(height));
}

btCollisionShape* CapsuleColliderComponent::GetCollisionShape() const
{
    return shape;
}
