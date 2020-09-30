#include "MoveComponent.h"

MoveComponent::MoveComponent()
{
	this->direction = { 0.f, 1.f, 0.f };
}

MoveComponent::~MoveComponent()
{
}

//void MoveComponent::Move(DirectX::XMFLOAT3 moveVector)
//{
//	dx::XMVECTOR move = dx::XMLoadFloat3(&moveVector);
//	dx::XMVECTOR newPos = dx::XMVectorAdd(GetOwner()->GetTransform().GetPosition(), move);
//	GetOwner()->GetTransform().SetPosition(newPos);
//}

void MoveComponent::Update(const float& deltaTime)
{
	DirectX::XMFLOAT3 pos;
	dx::XMStoreFloat3(&pos, GetOwner()->GetTransform().GetPosition());
	if (pos.y > 3.f || pos.y < -3.f)
	{
		this->direction.y *=  -1; //negative direction if outside 3,-3 range
	}
	GetOwner()->GetTransform().Translate(this->direction.x * deltaTime, this->direction.y * deltaTime, this->direction.z * deltaTime);
}
