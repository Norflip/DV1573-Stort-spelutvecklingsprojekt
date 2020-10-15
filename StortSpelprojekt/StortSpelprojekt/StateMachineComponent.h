#pragma once
#include <map>
#include "AIPatrol.h"
#include "AIIdle.h"
#include "AIAttack.h"
#include "Input.h"
#include <unordered_map>

enum class AIState
{
	none,
	idle,
	patrol,
	attack
};

class StateMachineComponent : public Component
{
	public:
		std::map<AIState, AIState> switchState =
		{
			{AIState::idle, AIState::patrol},
			{AIState::patrol, AIState::attack},
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