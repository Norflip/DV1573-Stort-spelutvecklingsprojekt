#include "StateMachineComponent.h"

StateMachineComponent::StateMachineComponent(AIState state) :
	currentState(state)
{
	
}

StateMachineComponent::~StateMachineComponent()
{
}

void StateMachineComponent::SetState(AIState state)
{
	if (currentState != AIState::none)
	{
		stateMap[currentState]->SetEnabled(false);
	}

	currentState = state;

	if (currentState != AIState::none)
	{
		stateMap[currentState]->SetEnabled(true);
	}
}

void StateMachineComponent::Update(const float& deltaTime)
{
	if (KEY_DOWN(K))
	{
		SetState(switchState[currentState]);
	}
}

void StateMachineComponent::RegisterState(AIState state, Component* comp)
{
	comp->SetEnabled(false);
	stateMap.insert({ state, comp });
}
