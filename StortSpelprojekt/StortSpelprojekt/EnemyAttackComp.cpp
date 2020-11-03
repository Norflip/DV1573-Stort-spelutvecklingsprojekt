#include "stdafx.h"
#include "EnemyAttackComp.h"
#include "PlayerComp.h"
EnemyAttackComp::EnemyAttackComp(PlayerComp* player)
	: player(player), playerRadius(2.5f)
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
	//this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setFrictionCoefficient(100.f);
	//this->rbComp->SetLinearDamping(0.9f);
	this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setRollingResistance(100.0f);
}

void EnemyAttackComp::Update(const float& deltaTime)
{
	timer.Update();
	ChasePlayer(deltaTime);
	UpdateAttackPlayer(deltaTime);
}

bool EnemyAttackComp::GetIsAttacking()
{
	return attackPlayer;
}

bool EnemyAttackComp::ChasePlayer(const float& deltaTime)
{

	if (KEY_PRESSED(U))
	{
		dx::XMVECTOR plPos = player->GetOwner()->GetTransform().GetPosition();
		dx::XMFLOAT3 pp;
		dx::XMStoreFloat3(&pp, plPos);
		pp.y += 50;

		//GetOwner()->GetTransform().SetPosition(dx::XMLoadFloat3(&pp));
		rbComp->SetPosition(dx::XMLoadFloat3(&pp));
	}

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

	if (length <= enemyStatsComp->GetChaseRadius() && length >= playerRadius)
	{
		chasePlayer = true;
		moveDir.x = normDir.x;
		moveDir.z = normDir.z;
	}

	if (length <= enemyStatsComp->GetAttackRadius())
	{
		attackPlayer = true;
	}
	if (chasePlayer)
	{
		dx::XMFLOAT3 vel = rbComp->GetLinearVelocity();
		
		dx::XMFLOAT3 move = { moveDir.x * enemyStatsComp->GetSpeed(), vel.y, moveDir.z * enemyStatsComp->GetSpeed() };

		rbComp->SetLinearVelocity(move);
		dx::XMVECTOR enemyPos = GetOwner()->GetTransform().GetPosition();
		dx::XMFLOAT3 eP;
		dx::XMStoreFloat3(&eP, enemyPos);
		std::cout << "x: " << eP.x << ", y: " << eP.y << ", z: " << eP.z << std::endl;

		//Billboarding x & z axis for enemy Rotation
		dx::XMVECTOR dirVec = dx::XMVector3Normalize(dx::XMVectorSubtract(GetOwner()->GetTransform().GetPosition(), player->GetOwner()->GetTransform().GetPosition()));
		dx::XMFLOAT3 dirToPlayer;
		dx::XMStoreFloat3(&dirToPlayer, dirVec);
		float angle = 180.f + atan2f(dirToPlayer.x, dirToPlayer.z) * (180.f / Math::PI);
		float rotation = angle * Math::ToRadians;
		dx::XMVECTOR right = GetOwner()->GetTransform().TransformDirection({ 1,0,0 });
		dx::XMVECTOR eulerRotation = dx::XMQuaternionMultiply(dx::XMQuaternionRotationAxis(right, 0), dx::XMQuaternionRotationAxis({ 0,1,0 }, rotation));
		GetOwner()->GetTransform().SetRotation(eulerRotation);
		rbComp->SetRotation(eulerRotation);
	}
	
	//Draw colliderbox
	//dx::XMFLOAT3 getBoxPos;
	//dx::XMStoreFloat3(&getBoxPos, GetOwner()->GetTransform().GetPosition());
	//dx::XMFLOAT3 boxSize = dx::XMFLOAT3(1, 2, 1);
	//dx::XMFLOAT3 boxColor = dx::XMFLOAT3(1, 1, 1);
	//DShape::DrawBox(dx::XMFLOAT3(getBoxPos.x, getBoxPos.y - 1.5f, getBoxPos.z), boxSize, boxColor);

	return chasePlayer;
}

void EnemyAttackComp::UpdateAttackPlayer(const float& deltaTime)
{
	if (attackPlayer)
	{
		if (timer.GetSeconds() >= enemyStatsComp->GetAttackSpeed())
		{
			timer.Restart();
			player->LoseHealth(enemyStatsComp->GetAttack());
		}
	}
}
