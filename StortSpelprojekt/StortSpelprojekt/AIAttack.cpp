#include "AIAttack.h"

AIAttack::AIAttack(CameraComponent* player)
	: player(player)
{

}

AIAttack::~AIAttack()
{
}

void AIAttack::Update(const float& deltaTime)
{
	UpdateAttackPlayer(deltaTime);
}

bool AIAttack::ChasePlayer(const float& deltaTime)
{
	bool chasePlayer = false;
	attackPlayer = false;

	DirectX::XMFLOAT3 enemyPos;
	dx::XMStoreFloat3(&enemyPos, GetOwner()->GetTransform().GetPosition());
	DirectX::XMFLOAT3 playerPos;
	dx::XMStoreFloat3(&playerPos, player->GetOwner()->GetTransform().GetPosition());

	dx::XMFLOAT3 distanceF = { enemyPos.x - playerPos.x, enemyPos.y - playerPos.y, enemyPos.z - playerPos.z };
	dx::XMFLOAT3 moveDir = { 0.0f, 0.0f, 0.0f };
	float stopAt = 2.0f;

	if (GetOwner()->GetComponent<StatsComponent>()->GetRadius() > distanceF.x && GetOwner()->GetComponent<StatsComponent>()->GetRadius() > distanceF.y &&
		GetOwner()->GetComponent<StatsComponent>()->GetRadius() > distanceF.z)
	{
		chasePlayer = true;
		if (distanceF.x > stopAt)
		{
			moveDir.x = -1.0f;
		}
		else if (distanceF.x < -stopAt)
		{
			moveDir.x = 1.0f;
		}

		if (distanceF.z > stopAt)
		{
			moveDir.z = -1.0f;
		}
		else if (distanceF.z < -stopAt)
		{
			moveDir.z = 1.0f;
		}
	}

	if (moveDir.x == 0.0f || moveDir.z == 0.0f)
		attackPlayer = true;

	GetOwner()->GetTransform().Translate(moveDir.x * GetOwner()->GetComponent<StatsComponent>()->GetSpeed() * deltaTime, 0.0f,
		moveDir.z * GetOwner()->GetComponent<StatsComponent>()->GetSpeed() * deltaTime);

	return chasePlayer;
}

void AIAttack::UpdateAttackPlayer(const float& deltaTime)
{
	if (ChasePlayer(deltaTime) && attackPlayer)
	{
	}
}
