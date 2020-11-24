#include "stdafx.h"
#include "EnemySMComp.h"

EnemySMComp::EnemySMComp(EnemyState state) :
	currentState(state)
{
}

EnemySMComp::~EnemySMComp()
{
}

void EnemySMComp::InitAnimation()
{
	skeletonComponent = GetOwner()->GetComponent<SkeletonMeshComponent>();
	attackComponent = GetOwner()->GetComponent<EnemyAttackComp>();
	statsComponent = GetOwner()->GetComponent<EnemyStatsComp>();
	enemyPatrolComp = GetOwner()->GetComponent<EnemyPatrolComp>();
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
}

void EnemySMComp::Initialize()
{
}

void EnemySMComp::Animate()
{

	if (currentState == EnemyState::ATTACK)
	{
		skeletonComponent->SetTrack(SkeletonStateMachine::RUN, false);

		if (attackComponent->GetIsAttacking())
		{
			skeletonComponent->SetTrack(SkeletonStateMachine::ATTACK, false);
		}
	}
	else if (currentState == EnemyState::PATROL)
	{
		skeletonComponent->SetTrack(SkeletonStateMachine::RUN, false);
	}
	/*else if (currentState == EnemyState::DEATH)
	{
		skeletonComponent->SetTrack(SkeletonStateMachine::DEATH, false);
	}*/
	else
	{
		skeletonComponent->SetTrack(SkeletonStateMachine::IDLE, false);
	}

}

void EnemySMComp::Update(const float& deltaTime)
{
	if (currentState != EnemyState::ATTACK && attackComponent->ChasePlayer())
	{
		SetState(EnemyState::ATTACK);
	}
	else if (currentState != EnemyState::IDLE && !attackComponent->ChasePlayer() && !enemyPatrolComp->GetIsMoving())
	{
		SetState(EnemyState::IDLE);
	}
	else if (currentState != EnemyState::PATROL && !attackComponent->ChasePlayer() && enemyPatrolComp->GetIsMoving())
	{
		SetState(EnemyState::PATROL);
	}

	if (GetOwner()->HasComponent<SkeletonMeshComponent>())
	{
		Animate();
	}
}

void EnemySMComp::RegisterState(EnemyState state, Component* comp)
{
	comp->SetEnabled(false);
	stateMap.insert({ state, comp });
}
