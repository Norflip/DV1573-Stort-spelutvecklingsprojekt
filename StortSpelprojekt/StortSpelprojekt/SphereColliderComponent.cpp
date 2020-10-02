#include "SphereColliderComponent.h"

SphereColliderComponent::SphereColliderComponent(float radius, float rX, float rY, float rZ, float rW, float pX, float pY, float pZ)
	:radius(radius), rX(rX), rY(rY), rZ(rZ), rW(rW), pX(pX), pY(pY), pZ(pZ)
{
}

void SphereColliderComponent::Initialize()
{
	shape = new btSphereShape(btScalar(radius));

	rotation = btQuaternion(btScalar(rX), btScalar(rY), btScalar(rZ), btScalar(rW));

	position = btVector3(btScalar(pX), btScalar(pY), btScalar(pZ));

	trans = btTransform(rotation, position);
}

btCollisionShape* SphereColliderComponent::GetCollisionShape() const
{
	return shape;
}

btTransform SphereColliderComponent::GetTransform() const
{
	return trans;
}
