#include "EnemySMComp.h"

EnemySMComp::EnemySMComp(EnemyState state) :
	currentState(state)
{
	
}

EnemySMComp::~EnemySMComp()
{
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
		GetOwner()->GetComponent<SkeletonMeshComponent>()->SetTrack(SkeletonStateMachine::RUN,false);
	}
	else if (currentState == EnemyState::IDLE)
	{
		GetOwner()->GetComponent<SkeletonMeshComponent>()->SetTrack(SkeletonStateMachine::IDLE,false);
	}
	else if (currentState == EnemyState::PATROL)
	{
		GetOwner()->GetComponent<SkeletonMeshComponent>()->SetTrack(SkeletonStateMachine::WALK,false);
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
