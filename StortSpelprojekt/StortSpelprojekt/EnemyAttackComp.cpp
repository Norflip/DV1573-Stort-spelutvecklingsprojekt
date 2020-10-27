#include "EnemyAttackComp.h"
#include "PlayerComp.h"
EnemyAttackComp::EnemyAttackComp(PlayerComp* player)
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
	chasePlayer = false;
	attackPlayer = false;

	DirectX::XMFLOAT3 enemyPos;
	dx::XMStoreFloat3(&enemyPos, GetOwner()->GetTransform().GetPosition());
	DirectX::XMFLOAT3 playerPos;
	dx::XMStoreFloat3(&playerPos, player->GetOwner()->GetTransform().GetPosition());

	dx::XMFLOAT3 distanceF = { enemyPos.x - playerPos.x, enemyPos.y - playerPos.y, enemyPos.z - playerPos.z };
	dx::XMFLOAT3 moveDir = { 0.0f, 0.0f, 0.0f };

	if(distanceF.x <= enemyStatsComp->GetRadius() && distanceF.z <= enemyStatsComp->GetRadius()
		&& distanceF.x >= -enemyStatsComp->GetRadius() && distanceF.z >= -enemyStatsComp->GetRadius())
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

	float length = (distanceF.x * distanceF.x + distanceF.y * distanceF.y + distanceF.z * distanceF.z);
	if (length < playerRadius * playerRadius)
	{
		attackPlayer = true;
	}

	GetOwner()->GetTransform().Translate(moveDir.x * enemyStatsComp->GetSpeed() * deltaTime, 0.0f,
		moveDir.z * enemyStatsComp->GetSpeed() * deltaTime);

	//rotation doesn't work
	//GetOwner()->GetTransform().SmoothRotation(playerPos, deltaTime, true);

	return chasePlayer;
}

void EnemyAttackComp::UpdateAttackPlayer(const float& deltaTime)
{
	if (ChasePlayer(deltaTime) && attackPlayer)
	{
		if (timer.GetSeconds() >= enemyStatsComp->GetAttackSpeed())
		{
			timer.Restart();
			player->LoseHealth(enemyStatsComp->GetAttack());
		}
	}
}
