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
		skeletonComponent->SetTrack(SkeletonStateMachine::BLENDED,false);
	}
	else if (currentState == EnemyState::IDLE)
	{
		skeletonComponent->SetTrack(SkeletonStateMachine::IDLE,false);
	}
	else if (currentState == EnemyState::PATROL)
	{
		skeletonComponent->SetTrack(SkeletonStateMachine::WALK,false);
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
