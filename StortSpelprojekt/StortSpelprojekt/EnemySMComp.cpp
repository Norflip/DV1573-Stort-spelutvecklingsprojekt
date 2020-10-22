#include "EnemySMComp.h"

EnemySMComp::EnemySMComp(EnemyState state) :
	currentState(state)
{
	
}

EnemySMComp::~EnemySMComp()
{
}

void EnemySMComp::initAnimation()
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

void EnemySMComp::Animate()
{

	if (currentState == EnemyState::ATTACK)
	{
		if (attackComponent->GetIsAttacking())
		{
			skeletonComponent->SetTrack(SkeletonStateMachine::ATTACK, false);
		}
		else
		{
			skeletonComponent->SetTrack(SkeletonStateMachine::RUN, false);
		}
		
	}
	else if (currentState == EnemyState::IDLE)
	{
		skeletonComponent->SetTrack(SkeletonStateMachine::IDLE,false);
	}
	else if (currentState == EnemyState::PATROL)
	{
		skeletonComponent->SetTrack(SkeletonStateMachine::WALK,false);
	}
	if (statsComponent->GetHealth()<=0)
	{
		skeletonComponent->SetTrack(SkeletonStateMachine::DEATH, true);
	}

}



void EnemySMComp::Update(const float& deltaTime)
{
	if (KEY_DOWN(K))
	{
		SetState(switchState[currentState]);
		
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
