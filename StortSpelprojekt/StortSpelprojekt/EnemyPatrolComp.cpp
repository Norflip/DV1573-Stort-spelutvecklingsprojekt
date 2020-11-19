#include "stdafx.h"
#include "EnemyPatrolComp.h"

EnemyPatrolComp::EnemyPatrolComp()
{
	this->randomVec = { 0.f,0.f,0.f }; //(target vector)
	this->timer = 0.f;
	this->randomIntervall = 0.f;
}


void EnemyPatrolComp::Initialize()
{
	this->enemyStatsComp = GetOwner()->GetComponent<EnemyStatsComp>();
	this->rbComp = GetOwner()->GetComponent<RigidBodyComponent>();
	this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setBounciness(0.f);
	this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setRollingResistance(100.0f);
}

EnemyPatrolComp::~EnemyPatrolComp()
{
}

void EnemyPatrolComp::Update(const float& deltaTime)
{
	//animation not implemented 
	//switching to another enemy state not implemented

	//update
	//dx::XMVECTOR posVec = GetOwner()->GetTransform().GetPosition();
	if (timer >= randomIntervall)
	{
		timer = 0.f;
		randomIntervall = (rand() % 10);
		randomVec.x = (rand() % 20 - 10);
		randomVec.z = (rand() % 20 - 10);
		this->isMoving = true;
		if (abs(randomVec.x) < 3.f && abs(randomVec.y) < 3.f)
		{
			randomVec.x = 0.f;
			randomVec.z = 0.f;
			randomIntervall = 0.f;
			this->isMoving = false;
		}
		
	}
	else
	{
		timer += deltaTime;
		dx::XMFLOAT3 vel = rbComp->GetLinearVelocity();
		dx::XMFLOAT3 move = { randomVec.x/10 * enemyStatsComp->GetSpeed(), vel.y, randomVec.z/10 * enemyStatsComp->GetSpeed() };
		rbComp->SetLinearVelocity(move);

		//fix rotation
		float angle = 180.f + atan2f(-randomVec.x, -randomVec.z) * (180.f / Math::PI);
		float rotation = angle * Math::ToRadians;
		dx::XMVECTOR right = GetOwner()->GetTransform().TransformDirection({ 1,0,0 });
		dx::XMVECTOR eulerRotation = dx::XMQuaternionMultiply(dx::XMQuaternionRotationAxis(right, 0), dx::XMQuaternionRotationAxis({ 0,1,0 }, rotation));
		GetOwner()->GetTransform().SetRotation(eulerRotation);
		rbComp->SetRotation(eulerRotation);
	}

}

bool EnemyPatrolComp::GetIsMoving() const
{
	return this->isMoving;
}

void EnemyPatrolComp::SetMoving(bool moving)
{
	this->isMoving = moving;
}
