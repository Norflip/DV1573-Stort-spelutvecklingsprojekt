#pragma once
#include <map>
#include "AIMove.h"
#include "AIIdle.h"
#include "AIAttack.h"
#include "Input.h"
#include <unordered_map>

enum class AIState
{
	none,
	idle,
	move,
	attack
};

class StateMachineComponent : public Component
{
	public:
		std::map<AIState, AIState> switchState =
		{
			{AIState::idle, AIState::move},
			{AIState::move, AIState::attack},
			{AIState::attack, AIState::idle}
		};

	public:
		StateMachineComponent(AIState state);
		virtual ~StateMachineComponent();

		void SetState(AIState state);

		void Update(const float& deltaTime);
		void RegisterState(AIState state, Component* comp);
	private:
		AIState currentState;
		std::unordered_map<AIState, Component*> stateMap;
};