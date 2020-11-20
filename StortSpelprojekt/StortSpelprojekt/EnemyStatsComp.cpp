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
}

EnemyStatsComp::EnemyStatsComp(float health, float movementSpeed, float chaseRadius, float attack, float attackSpeed, float attackRadius)
	: manager(manager), health(health), movementSpeed(movementSpeed), chaseRadius(chaseRadius), attack(attack), attackSpeed(attackSpeed), attackRadius(attackRadius)
{
}

EnemyStatsComp::~EnemyStatsComp(){}
