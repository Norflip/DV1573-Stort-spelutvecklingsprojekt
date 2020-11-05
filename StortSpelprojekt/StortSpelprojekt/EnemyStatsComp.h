#pragma once
#include "Component.h"

class EnemyStatsComp : public Component
{
	public:
		EnemyStatsComp();
		EnemyStatsComp(float health, float movementSpeed, float chaseRadius, float attack, float attackSpeed, float attackRadius);
		virtual ~EnemyStatsComp();
		float GetHealth() const { return health; }
		float GetSpeed() const { return movementSpeed; }
		float GetChaseRadius() const { return chaseRadius; }
		float GetAttack() const { return attack; }
		float GetAttackSpeed() const { return attackSpeed; }
		float GetAttackRadius() const { return attackRadius; }
		void LoseHealth(float damage) { health -= damage; }
		void SetHealth(float health) { this->health = health; }
		void AddHealth(float health) { this->health += health; }
		void SetAttack(float attack) { this->attack = attack; }
		void AddAttack(float attack) { this->attack += attack; }
		void SetAttackSpeed(float attackSpeed) { this->attackSpeed = attackSpeed; }
	private:
		float health;
		float movementSpeed;
		float chaseRadius;
		float attack;
		float attackSpeed;
		float attackRadius;
};