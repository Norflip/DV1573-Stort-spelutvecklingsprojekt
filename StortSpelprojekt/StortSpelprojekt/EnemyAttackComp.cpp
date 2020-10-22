#include "EnemyAttackComp.h"

EnemyAttackComp::EnemyAttackComp(CameraComponent* player)
	: player(player), playerRadius(2.0f)
{
	timer.Start();
}

EnemyAttackComp::~EnemyAttackComp()
{
}

void EnemyAttackComp::Initialize()
{
	enemyStatsComp = GetOwner()->GetComponent<EnemyStatsComp>();
}

void EnemyAttackComp::Update(const float& deltaTime)
{
	timer.Update();
	UpdateAttackPlayer(deltaTime);
}

bool EnemyAttackComp::GetIsAttacking()
{
	return attackPlayer;
}

bool EnemyAttackComp::ChasePlayer(const float& deltaTime)
{
	bool chasePlayer = false;
	attackPlayer = false;

	DirectX::XMFLOAT3 enemyPos;
	dx::XMStoreFloat3(&enemyPos, GetOwner()->GetTransform().GetPosition());
	DirectX::XMFLOAT3 playerPos;
	dx::XMStoreFloat3(&playerPos, player->GetOwner()->GetTransform().GetPosition());

	dx::XMFLOAT3 distanceF = { enemyPos.x - playerPos.x, enemyPos.y - playerPos.y, enemyPos.z - playerPos.z };
	dx::XMFLOAT3 moveDir = { 0.0f, 0.0f, 0.0f };

	if (enemyStatsComp->GetRadius() > distanceF.x && enemyStatsComp->GetRadius() > distanceF.y &&
		enemyStatsComp->GetRadius() > distanceF.z)
	{
		chasePlayer = true;
		if (distanceF.x > playerRadius)
		{
			moveDir.x = -1.0f;
		}
		else if (distanceF.x < -playerRadius)
		{
			moveDir.x = 1.0f;
		}

		if (distanceF.z > playerRadius)
		{
			moveDir.z = -1.0f;
		}
		else if (distanceF.z < -playerRadius)
		{
			moveDir.z = 1.0f;
		}
	}

	if (distanceF.x <= playerRadius && distanceF.z <= playerRadius
		&& distanceF.x >= -playerRadius && distanceF.z >= -playerRadius)
		attackPlayer = true;

	GetOwner()->GetTransform().Translate(moveDir.x * GetOwner()->GetComponent<EnemyStatsComp>()->GetSpeed() * deltaTime, 0.0f,
		moveDir.z * GetOwner()->GetComponent<EnemyStatsComp>()->GetSpeed() * deltaTime);

	return chasePlayer;
}

void EnemyAttackComp::UpdateAttackPlayer(const float& deltaTime)
{
	if (ChasePlayer(deltaTime) && attackPlayer)
	{
		if (timer.GetSeconds() >= GetOwner()->GetComponent<EnemyStatsComp>()->GetAttackSpeed())
		{
			timer.Restart();
			player->GetOwner()->GetComponent<EnemyStatsComp>()->LoseHealth(GetOwner()->GetComponent<EnemyStatsComp>()->GetAttack());

		}
	}
}
