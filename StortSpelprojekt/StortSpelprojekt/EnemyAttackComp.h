#pragma once
#include "Object.h"
#include "EnemyStatsComp.h"
#include "GameClock.h"
#include "Math.h"

class PlayerComp;
class EnemyAttackComp : public Component
{
	public:
		EnemyAttackComp(PlayerComp* player);
		virtual ~EnemyAttackComp();
		void Initialize();
		void Update(const float& deltaTime) override;
		bool GetChasePlayer() const {return chasePlayer;}
		bool ChasePlayer();
		bool GetIsAttacking();
	private:

		void UpdateEnemyPos(const float& deltaTime);
		void UpdateAttackPlayer(const float& deltaTime);

		PlayerComp* player;
		bool attackPlayer = false;
		GameClock timer;
		float playerRadius;
		EnemyStatsComp* enemyStatsComp;
		RigidBodyComponent* rbComp;
		bool chasePlayer = false;
};