#pragma once
#include "Object.h"

class AIMove : public Component
{
	public:
		AIMove();
		virtual ~AIMove();

		void Update(const float& deltaTime) override;
	private:
		dx::XMFLOAT3 direction;
};