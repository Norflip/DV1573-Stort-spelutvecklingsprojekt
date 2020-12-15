#include "stdafx.h"
#include "EnemySMComp.h"

EnemySMComp::EnemySMComp(EnemyState state) :
	currentState(state)
{
}

EnemySMComp::~EnemySMComp()
{
}

void EnemySMComp::InitAnimation(Object* playerObj)
{
	skeletonComponent = GetOwner()->GetComponent<SkeletonMeshComponent>();
	attackComponent = GetOwner()->GetComponent<EnemyAttackComp>();
	statsComponent = GetOwner()->GetComponent<EnemyStatsComp>();
	skeletonComponent->CreateCombinedAnimation(SkeletonStateMachine::RUN, SkeletonStateMachine::ATTACK, 5, 36);
	enemyPatrolComp = GetOwner()->GetComponent<EnemyPatrolComp>();
	player = playerObj;
}

void EnemySMComp::SetState(EnemyState state)
{
	if (currentState != EnemyState::NONE)
	{
		stateMap[currentState]->SetEnabled(false);
	}
	currentState = state;
	if (currentState != EnemyState::NONE)
	{
		stateMap[currentState]->SetEnabled(true);
	}
}

void EnemySMComp::Start()
{
	//enemyAttackComp = GetOwner()->GetComponent<EnemyAttackComp>();
	//skeletonComponent->CreateBlendedAnimation();
	
}

void EnemySMComp::Initialize()
{
	
}

void EnemySMComp::Animate()
{
	float length = 0;
	dx::XMVECTOR playerVec = player->GetTransform().GetPosition();
	dx::XMVECTOR enemyVec = GetOwner()->GetTransform().GetPosition();
	dx::XMFLOAT3 enemyPos;
	dx::XMStoreFloat3(&enemyPos, enemyVec);
	dx::XMStoreFloat(&length, dx::XMVector3Length(dx::XMVectorSubtract(playerVec, enemyVec)));

	if (statsComponent->GetHealth() <= 0.0f || length >= ENEMY_RADIUS_LIMIT)
	{
		SetState(EnemyState::IDLE);
		GetOwner()->GetComponent<Pathfinding>()->SetEnabled(false);
		
		skeletonComponent->SetTrack(SkeletonStateMachine::DEATH, true);

		if (skeletonComponent->GetDoneDeath())
		{
			if (statsComponent->GetHealth() <= 0.0f)
				MetaProgress::Instance().EnemyKilled(GetOwner()->GetName());

			std::cout << "enemy died.. health: " << statsComponent->GetHealth() << ", y: " << enemyPos.y <<", length from player: "<<length<< std::endl;
			statsComponent->GetManager()->RemoveEnemy(GetOwner());
		}


	}
	else
	{
		if (currentState == EnemyState::ATTACK)
		{

			//FIXA FAKTOOOOOOOOOORN

			factorValue = (15.f - 0.f);
			factorRange = (1.f - 0.f);
			finalFactor = (((length - 0.f) * factorRange) / factorValue) + 0.f;
			std::cout << "Factor Loop 1: " << finalFactor << std::endl;

			skeletonComponent->SetBlendingTracksAndFactor(SkeletonStateMachine::ATTACK, SkeletonStateMachine::RUN, finalFactor, true);
			skeletonComponent->SetTrack(SkeletonStateMachine::BLENDED, false);
			if (finalFactor <= 0.f)
			{
				factorValue = (5.f - 0.f);
				factorRange = (1.f - 0.f);
				finalFactor = (((length - 0.f) * factorRange) / factorValue) + 0.f;

				std::cout << "Factor Loop 2: " << finalFactor << std::endl;

	
			}
		
		/*	else
			{
				skeletonComponent->SetTrack(SkeletonStateMachine::RUN, false);
			}*/


		}
		else if (currentState == EnemyState::PATROL)
		{

			if (skeletonComponent->GetEnemyType() == EnemyType::BASE)
			{
				factorValue = (20.f - 15.f);
				factorRange = (1.f - 0.f);
				finalFactor = (((length - 15.f) * factorRange) / factorValue) + 0.f;


				skeletonComponent->SetBlendingTracksAndFactor(SkeletonStateMachine::RUN, SkeletonStateMachine::WALK, finalFactor, true);
				skeletonComponent->SetTrack(SkeletonStateMachine::BLENDED, false);
			}
			else
			{
				skeletonComponent->SetTrack(SkeletonStateMachine::RUN, false);
			}
			
		}

		else
		{

			if (skeletonComponent->GetEnemyType() == EnemyType::BASE)
			{
				factorValue = (25.f - 20.f);
				factorRange = (1.f - 0.f);
				finalFactor = (((length - 20.f) * factorRange) / factorValue) + 0.f;
				
				skeletonComponent->SetBlendingTracksAndFactor(SkeletonStateMachine::WALK, SkeletonStateMachine::IDLE, finalFactor, true);
				skeletonComponent->SetTrack(SkeletonStateMachine::BLENDED, false);
			}
			
			else
			{
				skeletonComponent->SetTrack(SkeletonStateMachine::IDLE, false);
			}
		}
	}
	

}

void EnemySMComp::Update(const float& deltaTime)
{
	
	
	if (currentState != EnemyState::ATTACK && attackComponent->ChasePlayer())
	{
		SetState(EnemyState::ATTACK);
		GetOwner()->GetComponent<Pathfinding>()->SetEnabled(true);
	}
	else if (currentState != EnemyState::IDLE && !attackComponent->ChasePlayer() && !enemyPatrolComp->GetIsMoving())
	{
		SetState(EnemyState::IDLE);
		GetOwner()->GetComponent<Pathfinding>()->SetEnabled(false);
	}
	else if (currentState != EnemyState::PATROL && !attackComponent->ChasePlayer() && enemyPatrolComp->GetIsMoving())
	{
		SetState(EnemyState::PATROL);
		GetOwner()->GetComponent<Pathfinding>()->SetEnabled(false);
	}
	
	if (skeletonComponent)
	{
		Animate();
	}
}

void EnemySMComp::RegisterState(EnemyState state, Component* comp)
{
	comp->SetEnabled(false);
	stateMap.insert({ state, comp });
}
