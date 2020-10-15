#pragma once
#include "Object.h"

class AIPatrol : public Component
{
	public:
		AIPatrol();
		virtual ~AIPatrol();

		void Update(const float& deltaTime) override;
	private:
		dx::XMFLOAT3 direction;
		float positionA = 3.0f, positionB = -3.0f;
};