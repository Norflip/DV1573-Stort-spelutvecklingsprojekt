#include "stdafx.h"
#include "EnemyStatsComp.h"
#include "EnemyManager.h"

EnemyStatsComp::EnemyStatsComp() : manager(nullptr)
{
	health = 0;
	movementSpeed = 0;
	chaseRadius = 0;
	attack = 0;
	attackSpeed = 0;
	attackRadius = 0;

	baseHealth = health;
	//baseSpeed = movementSpeed;
	//baseAttack = attack;
	//baseAttackSpeed = baseAttackSpeed;
}

EnemyStatsComp::EnemyStatsComp(float health, float movementSpeed, float chaseRadius, float attack, float attackSpeed, float attackRadius)
	: manager(manager), health(health), baseHealth(health),movementSpeed(movementSpeed), chaseRadius(chaseRadius), attack(attack), attackSpeed(attackSpeed), attackRadius(attackRadius)
{
}

EnemyStatsComp::~EnemyStatsComp(){}

void EnemyStatsComp::Reset()
{
	this->health = this->baseHealth;
	//this->movementSpeed = this->baseSpeed;
	//this->attack = this->baseAttack;
	//this->attackSpeed = this->baseAttackSpeed;
}
