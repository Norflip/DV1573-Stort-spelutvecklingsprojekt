#include "CapsuleColliderComponent.h"

CapsuleColliderComponent::CapsuleColliderComponent(float radius, float height, float rX, float rY, float rZ, float rW, float pX, float pY, float pZ)
    :radius(radius), height(height), rX(rX), rY(rY), rZ(rZ), rW(rW), pX(pX), pY(pY), pZ(pZ)
{
}

void CapsuleColliderComponent::Initialize()
{
    shape = new btCapsuleShape(btScalar(radius), btScalar(height));

    rotation = btQuaternion(btScalar(rX), btScalar(rY), btScalar(rZ), btScalar(rW));

    position = btVector3(btScalar(pX), btScalar(pY), btScalar(pZ));

    trans = btTransform(rotation, position);
}

btCollisionShape* CapsuleColliderComponent::GetCollisionShape() const
{
    return shape;
}

btTransform CapsuleColliderComponent::GetTransform() const
{
    return trans;
}
