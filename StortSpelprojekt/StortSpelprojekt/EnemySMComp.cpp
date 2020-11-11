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
	skeletonComponent =  GetOwner()->GetComponent<SkeletonMeshComponent>();
	attackComponent = GetOwner()->GetComponent<EnemyAttackComp>();
	statsComponent = GetOwner()->GetComponent<EnemyStatsComp>();
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
	enemyAttackComp = GetOwner()->GetComponent<EnemyAttackComp>();
}

void EnemySMComp::Initialize()
{
}

void EnemySMComp::Animate()
{
	
	if (currentState == EnemyState::ATTACK)
	{
		skeletonComponent->SetTrack(SkeletonStateMachine::RUN, false);
	}
	else
	{
		skeletonComponent->SetTrack(SkeletonStateMachine::IDLE,false);
	}
	
}

void EnemySMComp::Update(const float& deltaTime)
{
	if (currentState != EnemyState::ATTACK && enemyAttackComp->ChasePlayer())
	{
		SetState(EnemyState::ATTACK);
	}
	else if (currentState != EnemyState::IDLE && !enemyAttackComp->ChasePlayer())
	{
		SetState(EnemyState::IDLE);
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
