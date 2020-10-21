#pragma once
#include "Object.h"
#include "EnemyStatsComp.h"
#include "GameClock.h"

class PlayerAttackComp : public Component
{
	public:
		PlayerAttackComp(Object* enemy);
		virtual ~PlayerAttackComp();
		void Initialize();
		void Update(const float& deltaTime);
		
	private:
		bool CheckAttack();
		void UpdateAttack();

		Object* enemy;
		GameClock timer;
		EnemyStatsComp* playerStatsComp;
		EnemyStatsComp* enemyStatsComp;
};