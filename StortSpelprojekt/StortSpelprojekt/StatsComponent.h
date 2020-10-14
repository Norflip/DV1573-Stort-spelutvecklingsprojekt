#pragma once
#include "Component.h"

class StatsComponent : public Component
{
	public:
		StatsComponent();
		StatsComponent(float health, float speed, float radius, float attack);
		virtual ~StatsComponent();
		float GetHealth() const { return health; }
		float GetSpeed() const { return speed; }
		float GetRadius() const { return radius; }
		float GetAttack() const { return attack; }
		void LoseHealth(float damage) { health -= damage; }
		void SetHealth(float health) { this->health = health; }
		void AddHealth(float health) { this->health += health; }
		void SetAttack(float attack) { this->attack += attack; }
		void AddAttack(float attack) { this->attack += attack; }
	private:
		float health;
		float speed;
		float radius;
		float attack;
};