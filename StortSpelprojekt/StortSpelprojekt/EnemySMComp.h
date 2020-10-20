#pragma once
#include <map>
#include "EnemyPatrolComp.h"
#include "EnemyIdleComp.h"
#include "EnemyAttackComp.h"
#include "Input.h"
#include <unordered_map>

enum class EnemyState
{
	NONE,
	IDLE,
	PATROL,
	ATTACK
};

class EnemySMComp : public Component
{
	public:
		std::map<EnemyState, EnemyState> switchState =
		{
			{EnemyState::IDLE, EnemyState::PATROL},
			{EnemyState::PATROL, EnemyState::ATTACK},
			{EnemyState::ATTACK, EnemyState::IDLE}
		};

	public:
		EnemySMComp(EnemyState state);
		virtual ~EnemySMComp();

		void SetState(EnemyState state);

		void Update(const float& deltaTime);
		void RegisterState(EnemyState state, Component* comp);
	private:
		EnemyState currentState;
		std::unordered_map<EnemyState, Component*> stateMap;
};