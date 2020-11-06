#include "stdafx.h"
#include "PlayerAttackComp.h"

PlayerAttackComp::PlayerAttackComp(Object* enemy)
	: enemy(enemy)
{
	timer.Start();
}

PlayerAttackComp::~PlayerAttackComp()
{
}

void PlayerAttackComp::Initialize()
{
	playerStatsComp = GetOwner()->GetComponent<PlayerComp>();
	enemyStatsComp = enemy->GetComponent<EnemyStatsComp>();
}

void PlayerAttackComp::Update(const float& deltaTime)
{
	timer.Update();
	UpdateAttack();
}

bool PlayerAttackComp::CheckAttack()
{
	bool attackEnemy = false;

	if (enemy != nullptr)
	{
		DirectX::XMFLOAT3 playerPos;
		dx::XMStoreFloat3(&playerPos, GetOwner()->GetTransform().GetPosition());
		DirectX::XMFLOAT3 enemyPos;
		dx::XMStoreFloat3(&enemyPos, enemy->GetTransform().GetPosition());

		dx::XMFLOAT3 distanceF = { enemyPos.x - playerPos.x, enemyPos.y - playerPos.y, enemyPos.z - playerPos.z };
		//if (distanceF.x <= playerStatsComp->GetRadius() 
		//	&& distanceF.z <= playerStatsComp->GetRadius()
		//	&& distanceF.x >= -playerStatsComp->GetRadius() 
		//	&& distanceF.z >= -playerStatsComp->GetRadius())
		//	attackEnemy = true;

		float length = (distanceF.x * distanceF.x + distanceF.y * distanceF.y + distanceF.z * distanceF.z);
		if (length < playerStatsComp->GetRadius() * playerStatsComp->GetRadius())
			attackEnemy = true;
	}
	return attackEnemy;
}

void PlayerAttackComp::UpdateAttack()
{
	if (CheckAttack())
	{
		if (timer.GetSeconds() >= playerStatsComp->GetAttackSpeed())
		{
			timer.Restart();
			enemyStatsComp->LoseHealth(playerStatsComp->GetAttack());
		}
	}
}
