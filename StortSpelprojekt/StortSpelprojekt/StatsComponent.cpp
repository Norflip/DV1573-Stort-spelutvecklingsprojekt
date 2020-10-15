#include "StatsComponent.h"

StatsComponent::StatsComponent()
{
	health = 0;
	movementSpeed = 0;
	radius = 0;
	attack = 0;
	attackSpeed = 0;
}

StatsComponent::StatsComponent(float health, float movementSpeed, float radius, float attack, float attackSpeed)
	: health(health), movementSpeed(movementSpeed), radius(radius), attack(attack), attackSpeed(attackSpeed)
{
}

StatsComponent::~StatsComponent(){}
