#include "MoveComponent.h"

MoveComponent::MoveComponent()
{

	DirectX::XMFLOAT3 dir = { 0, 1.f, 0 };
	this->direction = dx::XMLoadFloat3(&dir);
}

MoveComponent::~MoveComponent()
{
}

void MoveComponent::Update(const float& deltaTime)
{
	DirectX::XMFLOAT3 pos;
	dx::XMStoreFloat3(&pos, GetOwner()->GetTransform().GetPosition());

	if (pos.y > 1.f || pos.y < -1.f)
	{
		this->direction = dx::XMVectorScale(this->direction, -1);
	}

	dx::XMVECTOR move = dx::XMVectorScale(this->direction, deltaTime);
	dx::XMVECTOR newPos = dx::XMVectorAdd(GetOwner()->GetTransform().GetPosition(), move);
	GetOwner()->GetTransform().SetPosition(newPos);

	newPos = { 0,0,0 };
	move = { 0,0,0 };
	pos = { 0,0,0 };
}
