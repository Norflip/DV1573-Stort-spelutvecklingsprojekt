#pragma once
#include "Object.h"
#include "StatsComponent.h"

class AIAttack : public Component
{
	public:
		AIAttack(CameraComponent* player);
		virtual ~AIAttack();
		void Update(const float& deltaTime) override;
	private:
		CameraComponent* player;
};