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
	this->enemyStatsComp = GetOwner()->GetComponent<EnemyStatsComp>();
	this->rbComp = GetOwner()->GetComponent<RigidBodyComponent>();
	this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setBounciness(0.f);
	this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setFrictionCoefficient(100.f);
	this->rbComp->SetLinearDamping(0.9f);
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

	if (length < playerRadius)
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


	dx::XMVECTOR pos = GetOwner()->GetTransform().GetPosition(); 
	rbComp->SetPosition(pos); //make enemy velocity based later??

	rbComp->SetRotation(eulerRotation);

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
