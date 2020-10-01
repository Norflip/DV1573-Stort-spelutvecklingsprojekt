#include "BoxColliderComponent.h"

BoxColliderComponent::BoxColliderComponent(float x, float y, float z)
    :x(x), y(y), z(z)
{
}

void BoxColliderComponent::Initialize()
{
    shape = new btBoxShape(btVector3(x, y, z));
}

btCollisionShape* BoxColliderComponent::GetCollisionShape() const
{
    return shape;
}
