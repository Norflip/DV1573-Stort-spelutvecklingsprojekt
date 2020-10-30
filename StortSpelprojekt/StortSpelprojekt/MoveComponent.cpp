#include "stdafx.h"
#include "MoveComponent.h"

MoveComponent::MoveComponent()
{
	direction = { 0, 1.f, 0 };
}

MoveComponent::~MoveComponent()
{
}

void MoveComponent::Update(const float& deltaTime)
{
	DirectX::XMFLOAT3 pos;
	dx::XMStoreFloat3(&pos, GetOwner()->GetTransform().GetPosition());
  
	if (pos.y > 3.f || pos.y < -3.f)
	{
		direction.x *= -1;
		direction.y *= -1;
		direction.z *= -1;
	}

	/*dx::XMVECTOR move = dx::XMVectorScale(dx::XMLoadFloat3(&this->direction), deltaTime);
	dx::XMVECTOR newPos = dx::XMVectorAdd(GetOwner()->GetTransform().GetPosition(), move);
	GetOwner()->GetTransform().SetPosition(newPos);*/
	GetOwner()->GetTransform().Translate(this->direction.x * deltaTime, this->direction.y * deltaTime, this->direction.z * deltaTime);

}
