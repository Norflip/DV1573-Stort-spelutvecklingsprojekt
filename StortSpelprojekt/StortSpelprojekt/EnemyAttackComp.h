#pragma once
#include "Object.h"
#include "EnemyStatsComp.h"
#include "GameClock.h"

class PlayerComp;
class EnemyAttackComp : public Component
{
	public:
		EnemyAttackComp(PlayerComp* player);
		virtual ~EnemyAttackComp();
		void Initialize();
		void Update(const float& deltaTime) override;
		bool GetChasePlayer() const {return chasePlayer;}
	private:
		bool ChasePlayer(const float& deltaTime);
		void UpdateAttackPlayer(const float& deltaTime);

		PlayerComp* player;
		bool attackPlayer = false;
		GameClock timer;
		float playerRadius;
		EnemyStatsComp* enemyStatsComp;
		bool chasePlayer = false;
};