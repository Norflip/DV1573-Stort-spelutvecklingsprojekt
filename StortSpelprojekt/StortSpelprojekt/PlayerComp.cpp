#include "PlayerComp.h"

PlayerComp::PlayerComp()
{
	health = 100;
	movementSpeed = 10;
	attack = 0;
	attackSpeed = 0;

	/*this->fuelDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("fuelDippingBar"));
	this->foodDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("foodDippingBar"));
	this->healthDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("healthDippingBar"));*/
}

PlayerComp::PlayerComp(GUIManager* guimanager, float health, float movementSpeed, float radius, float attack, float attackSpeed)
{
	this->guiMan = guimanager;
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
	this->fuelBurnPerMeter = 0.7f;
	this->fuel = 100.f;

	this->fuelDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("fuelDippingBar"));
	this->foodDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("foodDippingBar"));
	this->healthDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("healthDippingBar"));
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
		std::cout << fuel <<std::endl;
		// loose food
		food -= GameClock::Instance().GetFrameTime() / 1000 * foodLossPerSecond;
		//std::cout << food<<std::endl;

		// make better later
		if (food < 0 || fuel < 0)
			std::cout <<"GG";
	}
	
	
	// Fuel drop
	fuelDippingBar->SetScaleDipping(ReverseAndClamp(fuel));

	// Food drop
	foodDippingBar->SetScaleDipping(ReverseAndClamp(food));

	// Health dipping when hit by enemy
	//healthDippingBar->SetScaleDipping(0);
}

float PlayerComp::ReverseAndClamp(float inputValue)
{
	return 1.0f - (inputValue / 100.0f);
}
