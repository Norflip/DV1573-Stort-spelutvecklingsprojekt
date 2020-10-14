#pragma once
#include "Component.h"

class StatsComponent : public Component
{
	public:
		StatsComponent(float health, float speed, float radius);
		virtual ~StatsComponent();
		float GetHealth() const { return health; }
		float GetSpeed() const { return speed; }
		float GetRadius() const { return radius; }
	private:
		float health;
		float speed;
		float radius;
};