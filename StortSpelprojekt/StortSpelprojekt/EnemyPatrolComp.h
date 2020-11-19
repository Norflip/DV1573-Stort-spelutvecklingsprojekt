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
		bool GetIsMoving()const;
		void SetMoving(bool moving);
	private:
		dx::XMFLOAT3 randomVec; //(target vector)
		float timer;
		float randomIntervall;
		bool isMoving;

		RigidBodyComponent* rbComp;
		EnemyStatsComp* enemyStatsComp;
};