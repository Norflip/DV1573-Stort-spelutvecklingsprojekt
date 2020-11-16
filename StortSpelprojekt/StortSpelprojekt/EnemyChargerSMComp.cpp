#include "stdafx.h"
#include "EnemyChargerSMComp.h"

EnemyChargerSMComp::EnemyChargerSMComp(EnemyChargerState state) : currentState(state)
{
	loadRun = false;
}

EnemyChargerSMComp::~EnemyChargerSMComp()
{
}

void EnemyChargerSMComp::SetState(EnemyChargerState state)
{
	if (currentState != EnemyChargerState::NONE)
	{
		stateMap[currentState]->SetEnabled(false);
	}
	currentState = state;
	if (currentState != EnemyChargerState::NONE)
	{
		stateMap[currentState]->SetEnabled(true);
	}
}

void EnemyChargerSMComp::Start()
{
}

void EnemyChargerSMComp::Initialize()
{
}

void EnemyChargerSMComp::InitAnimation()
{
	skeletonComponent = GetOwner()->GetComponent<SkeletonMeshComponent>();
	enemyAttackComp = GetOwner()->GetComponent<EnemyAttackComp>();
	statsComponent = GetOwner()->GetComponent<EnemyStatsComp>();
}

void EnemyChargerSMComp::Update(const float& deltaTime)
{
	if (currentState != EnemyChargerState::ATTACK && enemyAttackComp->ChasePlayer())
	{
		SetState(EnemyChargerState::ATTACK);

	}
	else if (currentState != EnemyChargerState::IDLE && !enemyAttackComp->ChasePlayer())
	{
		SetState(EnemyChargerState::IDLE);
	}

	if (GetOwner()->HasComponent<SkeletonMeshComponent>())
	{
		Animate();
	}
}

void EnemyChargerSMComp::RegisterState(EnemyChargerState state, Component* comp)
{
	comp->SetEnabled(false);
	stateMap.insert({ state, comp });
}

void EnemyChargerSMComp::Animate()
{
	if (currentState == EnemyChargerState::ATTACK)
	{
		if (!loadRun)
		{
			skeletonComponent->SetTrack(SkeletonStateMachine::LOAD, true);
			loadRun = true;
		}

		if(loadRun)
			skeletonComponent->SetTrack(SkeletonStateMachine::RUN, false);

		if (enemyAttackComp->GetIsAttacking())
		{
			skeletonComponent->SetTrack(SkeletonStateMachine::ATTACK, false);
		}
	}
	else if (currentState == EnemyChargerState::IDLE)
	{
		if (loadRun)
		{
			skeletonComponent->SetTrack(SkeletonStateMachine::UNLOAD, true);
			loadRun = false;
		}

		if(!loadRun)
			skeletonComponent->SetTrack(SkeletonStateMachine::IDLE, false);
	}
	else
	{
		if (loadRun)
		{
			skeletonComponent->SetTrack(SkeletonStateMachine::UNLOAD, true);
			loadRun = false;
		}

		if (!loadRun)
			skeletonComponent->SetTrack(SkeletonStateMachine::IDLE, false);
	}
}
