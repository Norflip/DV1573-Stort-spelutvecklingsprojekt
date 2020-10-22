#pragma once
#include "Object.h"
#include "EnemyStatsComp.h"
#include "GameClock.h"

class EnemyAttackComp : public Component
{
	public:
		EnemyAttackComp(CameraComponent* player);
		virtual ~EnemyAttackComp();
		void Initialize();
		void Update(const float& deltaTime) override;
		bool GetIsAttacking();
	private:
		bool ChasePlayer(const float& deltaTime);
		void UpdateAttackPlayer(const float& deltaTime);

		CameraComponent* player;
		bool attackPlayer = false;
		GameClock timer;
		float playerRadius;
		EnemyStatsComp* enemyStatsComp;
};