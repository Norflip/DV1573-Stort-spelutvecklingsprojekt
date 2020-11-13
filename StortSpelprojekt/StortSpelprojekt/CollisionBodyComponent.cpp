#include "stdafx.h"
#include "CollisionBodyComponent.h"

//void CollisionBodyComponent::m_initializeBody(Physics* physics)
//{
//	if (!initialized)
//	{
//		this->physics = physics;
//		Transform& transform = GetOwner()->GetTransform();
//		currentTransform = ConvertToBtTransform(transform);
//		previousTransform = currentTransform;
//
//		//rp::BodyType bodyType = static_cast<rp::BodyType>((int)type);
//		rp::BodyType bodyType = rp::BodyType::STATIC;
//
//		body = physics->GetWorld()->createCollisionBody(currentTransform);
//		
//	}
//}

void CollisionBodyComponent::Initialize()
{

}

void CollisionBodyComponent::Update(const float& deltaTime)
{

}

rp::Transform CollisionBodyComponent::ConvertToBtTransform(const Transform& transform) const
{
	rp::Transform temp;

	dx::XMFLOAT3 tmpPosition;
	dx::XMStoreFloat3(&tmpPosition, transform.GetPosition());
	temp.setPosition(rp::Vector3(tmpPosition.x, tmpPosition.y, tmpPosition.z));

	dx::XMFLOAT4 tmpRotation;
	dx::XMStoreFloat4(&tmpRotation, transform.GetRotation());
	temp.setOrientation(rp::Quaternion(tmpRotation.x, tmpRotation.y, tmpRotation.z, tmpRotation.w));
	return temp;
}
