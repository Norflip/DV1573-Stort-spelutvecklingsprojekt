#pragma once
#include "Object.h"
#include "EnemyStatsComp.h"
#include "GameClock.h"
#include "PlayerComp.h"
class PlayerComp;
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
		PlayerComp* playerStatsComp;
		EnemyStatsComp* enemyStatsComp;
		
};