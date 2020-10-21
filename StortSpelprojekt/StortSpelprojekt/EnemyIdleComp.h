#pragma once
#include "Object.h"

class EnemyIdleComp : public Component
{
	public:
		EnemyIdleComp();
		virtual ~EnemyIdleComp();
		void Update(const float& deltaTime) override;
	private:
};