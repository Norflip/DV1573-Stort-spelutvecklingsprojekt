#pragma once
#include "Object.h"
#include "EnemyStatsComp.h"

class EnemyPatrolComp : public Component
{
	public:
		EnemyPatrolComp();
		virtual ~EnemyPatrolComp();
		void Initialize();
		void Update(const float& deltaTime) override;
	private:
		dx::XMFLOAT3 randomVec; //(target vector)
		float timer;
		float randomIntervall;

		RigidBodyComponent* rbComp;
		EnemyStatsComp* enemyStatsComp;
};