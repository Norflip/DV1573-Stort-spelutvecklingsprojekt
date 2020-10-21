#pragma once
#include "Component.h"

class EnemyStatsComp : public Component
{
	public:
		EnemyStatsComp();
		EnemyStatsComp(float health, float movementSpeed, float radius, float attack, float attackSpeed);
		virtual ~EnemyStatsComp();
		float GetHealth() const { return health; }
		float GetSpeed() const { return movementSpeed; }
		float GetRadius() const { return radius; }
		float GetAttack() const { return attack; }
		float GetAttackSpeed() const { return attackSpeed; }
		void LoseHealth(float damage) { health -= damage; }
		void SetHealth(float health) { this->health = health; }
		void AddHealth(float health) { this->health += health; }
		void SetAttack(float attack) { this->attack = attack; }
		void AddAttack(float attack) { this->attack += attack; }
		void SetAttackSpeed(float attackSpeed) { this->attackSpeed = attackSpeed; }
	private:
		float health;
		float movementSpeed;
		float radius;
		float attack;
		float attackSpeed;
};