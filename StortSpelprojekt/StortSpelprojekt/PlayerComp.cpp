#include "PlayerComp.h"

PlayerComp::PlayerComp()
{
	health = 100;
	movementSpeed = 10;
	attack = 0;
	attackSpeed = 0;
}

PlayerComp::PlayerComp(float health, float movementSpeed, float radius, float attack, float attackSpeed)
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

	// defaulting some shit
	this->foodLossPerSecond = 0.3f;
	this->food = 100.f;
	this->fuelBurnPerMeter = 0.1f;
	this->fuel = 100.f;
}

PlayerComp::~PlayerComp()
{

}

void PlayerComp::Update(const float& deltaTime)
{
	//temp fix for wierd clock start at 
	if (GameClock::Instance().GetFrameTime() / 1000 < 5.f)
	{
		//loose fuel
		//std::cout << GameClock::Instance().GetFrameTime() / 1000;
		fuel -= GameClock::Instance().GetFrameTime() / 1000 * fuelBurnPerMeter;
		//std::cout << food<<std::endl;
		// loose food
		food -= GameClock::Instance().GetFrameTime() / 1000 * foodLossPerSecond;
		//std::cout << food<<std::endl;

		// make better later
		if (food < 0 || fuel < 0)
			std::cout <<"GG";
	}
}