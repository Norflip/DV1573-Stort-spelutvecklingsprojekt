#include "SphereColliderComponent.h"

SphereColliderComponent::SphereColliderComponent(float radius)
	:radius(radius)
{
}

void SphereColliderComponent::Initialize()
{
	shape = new btSphereShape(btScalar(radius));
}

btCollisionShape* SphereColliderComponent::GetCollisionShape() const
{
	return shape;
}
