#include "BoxColliderComponent.h"

BoxColliderComponent::BoxColliderComponent(float x, float y, float z, float rX, float rY, float rZ, float rW, float pX, float pY, float pZ)
    :x(x), y(y), z(z), rX(rX), rY(rY), rZ(rZ), rW(rW), pX(pX), pY(pY), pZ(pZ)
{
}

void BoxColliderComponent::Initialize()
{
    shape = new btBoxShape(btVector3(x, y, z));

    rotation = btQuaternion(btScalar(rX), btScalar(rY), btScalar(rZ), btScalar(rW));

    position = btVector3(btScalar(pX), btScalar(pY), btScalar(pZ));

    trans = btTransform(rotation, position);
}

btCollisionShape* BoxColliderComponent::GetCollisionShape() const
{
    return shape;
}

btTransform BoxColliderComponent::GetTransform() const
{
    return trans;
}
