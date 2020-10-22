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

void EnemySMComp::Update(const float& deltaTime)
{
	//if (KEY_DOWN(K))
	//{
	//	SetState(switchState[currentState]);
	//}
	if (GetOwner()->GetComponent<EnemyAttackComp>()->GetChasePlayer())
	{
		SetState(EnemyState::ATTACK);
	}
	else
	{
		SetState(EnemyState::IDLE);
	}
}

void EnemySMComp::RegisterState(EnemyState state, Component* comp)
{
	comp->SetEnabled(false);
	stateMap.insert({ state, comp });
}
