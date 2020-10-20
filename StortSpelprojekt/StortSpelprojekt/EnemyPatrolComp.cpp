#include "EnemyPatrolComp.h"

EnemyPatrolComp::EnemyPatrolComp()
{
	direction = { 0, 1.f, 0 };
}

EnemyPatrolComp::~EnemyPatrolComp()
{
}

void EnemyPatrolComp::Update(const float& deltaTime)
{
	DirectX::XMFLOAT3 pos;
	dx::XMStoreFloat3(&pos, GetOwner()->GetTransform().GetPosition());

	if (pos.y > positionA || pos.y < positionB)
	{
		direction.x *= -1;
		direction.y *= -1;
		direction.z *= -1;
	}

	dx::XMVECTOR move = dx::XMVectorScale(dx::XMLoadFloat3(&this->direction), deltaTime);
	dx::XMVECTOR newPos = dx::XMVectorAdd(GetOwner()->GetTransform().GetPosition(), move);
	GetOwner()->GetTransform().SetPosition(newPos);
}
