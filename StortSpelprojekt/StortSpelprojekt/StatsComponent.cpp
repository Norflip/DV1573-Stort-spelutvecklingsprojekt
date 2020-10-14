#include "StatsComponent.h"

StatsComponent::StatsComponent()
{
	health = 0;
	speed = 0;
	radius = 0;
	attack = 0;
}

StatsComponent::StatsComponent(float health, float speed, float radius, float attack)
	: health(health), speed(speed), radius(radius), attack(attack)
{
}

StatsComponent::~StatsComponent(){}
