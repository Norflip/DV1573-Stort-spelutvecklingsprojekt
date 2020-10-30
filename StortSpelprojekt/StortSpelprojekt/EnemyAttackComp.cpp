#include "stdafx.h"
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

	//DirectX::XMFLOAT3 enemyPos;
	//dx::XMStoreFloat3(&enemyPos, GetOwner()->GetTransform().GetPosition());
	//DirectX::XMFLOAT3 playerPos;
	//dx::XMStoreFloat3(&playerPos, player->GetOwner()->GetTransform().GetPosition());

	//dx::XMFLOAT3 distanceF = { enemyPos.x - playerPos.x, enemyPos.y - playerPos.y, enemyPos.z - playerPos.z };
	dx::XMFLOAT3 moveDir = { 0.0f, 0.0f, 0.0f };


	//testing better radius calculations
	dx::XMVECTOR moveVec = dx::XMVectorSubtract(player->GetOwner()->GetTransform().GetPosition(), GetOwner()->GetTransform().GetPosition());
	dx::XMVECTOR normVec = dx::XMVector3Normalize(moveVec); //now contains normalized vector from enemy to player
	dx::XMFLOAT3 normDir;
	dx::XMStoreFloat3(&normDir, normVec);

	dx::XMVECTOR lenVec = dx::XMVector3Length(moveVec);//now contatins length
	float length;
	dx::XMStoreFloat(&length, lenVec);

	if (length <= enemyStatsComp->GetRadius())
	{
		chasePlayer = true;
		moveDir.x = normDir.x;
		moveDir.z = normDir.z;
	}

	//if(distanceF.x <= enemyStatsComp->GetRadius() && distanceF.z <= enemyStatsComp->GetRadius()
	//	&& distanceF.x >= -enemyStatsComp->GetRadius() && distanceF.z >= -enemyStatsComp->GetRadius())
	//{
	//	chasePlayer = true;
	//	if (distanceF.x > playerRadius)
	//	{
	//		moveDir.x = -1.0f;
	//	}
	//	else if (distanceF.x < -playerRadius)
	//	{
	//		moveDir.x = 1.0f;
	//	}

	//	if (distanceF.z > playerRadius)
	//	{
	//		moveDir.z = -1.0f;
	//	}
	//	else if (distanceF.z < -playerRadius)
	//	{
	//		moveDir.z = 1.0f;
	//	}
	//}

	//float length = (distanceF.x * distanceF.x + distanceF.y * distanceF.y + distanceF.z * distanceF.z);
	if (length < playerRadius)//playerRadius * playerRadius)
	{
		attackPlayer = true;
	}

	GetOwner()->GetTransform().Translate(moveDir.x * enemyStatsComp->GetSpeed() * deltaTime, 0.0f,
		moveDir.z * enemyStatsComp->GetSpeed() * deltaTime);


	//Billboarding x & z axis for enemy Rotation
	dx::XMVECTOR dirVec = dx::XMVector3Normalize(dx::XMVectorSubtract(GetOwner()->GetTransform().GetPosition(), player->GetOwner()->GetTransform().GetPosition()));
	dx::XMFLOAT3 dirToPlayer;
	dx::XMStoreFloat3(&dirToPlayer, dirVec);
	float angle = 180.f + atan2f(dirToPlayer.x, dirToPlayer.z) * (180.f / Math::PI);
	float rotation = angle * Math::ToRadians;
	dx::XMVECTOR right = GetOwner()->GetTransform().TransformDirection({ 1,0,0 });
	dx::XMVECTOR eulerRotation = dx::XMQuaternionMultiply(dx::XMQuaternionRotationAxis(right, 0), dx::XMQuaternionRotationAxis({ 0,1,0 }, rotation));
	GetOwner()->GetTransform().SetRotation(eulerRotation);


	dx::XMVECTOR a = GetOwner()->GetTransform().GetPosition(); //variable a??
	GetOwner()->GetComponent<RigidBodyComponent>()->SetPosition(a);


	return chasePlayer;
}

void EnemyAttackComp::UpdateAttackPlayer(const float& deltaTime)
{
	if (ChasePlayer(deltaTime) && attackPlayer)
	{
		// byebye
		if (timer.GetSeconds() >= enemyStatsComp->GetAttackSpeed())
		{
			timer.Restart();
			player->LoseHealth(enemyStatsComp->GetAttack());
		}
	}
}
