#pragma once
#include "Object.h"

class EnemyPatrolComp : public Component
{
	public:
		EnemyPatrolComp();
		virtual ~EnemyPatrolComp();

		void Update(const float& deltaTime) override;
	private:
		dx::XMFLOAT3 direction;
		float positionA = 3.0f, positionB = -3.0f;
};