#include "PlayerStatComp.h"

PlayerStatComp::PlayerStatComp()
{
	health = 100;
	movementSpeed = 10;
	attack = 0;
	attackSpeed = 0;
}

PlayerStatComp::PlayerStatComp(float health, float movementSpeed, float radius, float attack, float attackSpeed)
{
	this->health = health;
	this->attack = attack;
	this->attackSpeed = attackSpeed;
	this->fuel = fuel;
	this->food = food;
	this->currentWeapon = currentWeapon;
	this->movementSpeed = movementSpeed;
	this->crouchSpeed = crouchSpeed;
	this->SprintSpeed = SprintSpeed;
	this->foodLossPerSecond = foodLossPerSecond;
	this->fuelBurnPerMeter = fuelBurnPerMeter;
}

PlayerStatComp::~PlayerStatComp()
{
}
