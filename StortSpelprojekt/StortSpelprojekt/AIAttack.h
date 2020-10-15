#pragma once
#include "Object.h"
#include "StatsComponent.h"
#include "GameClock.h"

class AIAttack : public Component
{
	public:
		AIAttack(CameraComponent* player);
		virtual ~AIAttack();
		void Update(const float& deltaTime) override;
	private:
		bool ChasePlayer(const float& deltaTime);
		void UpdateAttackPlayer(const float& deltaTime);

		CameraComponent* player;
		bool attackPlayer = false;
		GameClock timer;
		float playerRadius = 2.0f;
};