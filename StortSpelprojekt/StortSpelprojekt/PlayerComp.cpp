#include "PlayerComp.h"

//PlayerComp::PlayerComp() 
//{
//	health = 100;
//	movementSpeed = 10;
//	attack = 0;
//	attackSpeed = 0;
//	radius = 15;	// dont know ffs
//	/*this->fuelDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("fuelDippingBar"));
//	this->foodDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("foodDippingBar"));
//	this->healthDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("healthDippingBar"));*/
//
//	foodEmpty = false;
//	gg = false;
//}

PlayerComp::PlayerComp(Renderer* renderer, CameraComponent* camComp, Physics& phys, GUIManager* guimanager, float health, float movementSpeed, float radius, float attack, float attackSpeed) : phy(phys)
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
	this->renderer = renderer;

	this->radius = radius;	// dont know ffs

	// defaulting some shit
	this->swapScene = NEXT_SCENE::GAME;
	this->foodLossPerSecond = 0.3f;
	this->food = 100.f;
	this->fuelBurnPerMeter = 0.7f;
	this->fuel = 100.f;
	this->healthLossPerSecond = 0.5f;
	this->health = 80.0f;

	this->fuelDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("fuelDippingBar"));
	this->foodDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("foodDippingBar"));
	this->healthDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("healthDippingBar"));

	foodEmpty = false;
	gg = false;	
	//this->GetOwner()->GetComponent<CameraComponent>()
	p.x = renderer->GetOutputWindow()->GetWidth() / 2;
	p.y = renderer->GetOutputWindow()->GetHeight() / 2;

	//Object* temp = this->GetOwner();
	cam = camComp;
	// per frame shit
	this->pappa = 2.0f;
	
}

PlayerComp::~PlayerComp()
{

}

void PlayerComp::Update(const float& deltaTime)
{
	
	Ray ray = cam->MouseToRay(p.x, p.y);
	//std::cout << p.x << ", " << p.y << std::endl;

	if (LMOUSE_PRESSED)
	{
		Physics& phy = Physics::Instance();

		//DShape::DrawLine(ray.origin, ray.GetPoint(1000.0f), { 1,1,0 });

		
		if (phy.RaytestSingle(ray, pappa, hit, FilterGroups::PICKUPS))
		{
			//DShape::DrawLine(ray.origin, hit.position, { 1,1,0 });
			//DShape::DrawSphere(hit.position, 1.0f, { 0, 0, 1 });

			if (hit.object != nullptr)
			{
				std::cout << "YALLA" << std::endl;
				
				Type pickupType = hit.object->GetComponent<PickupComponent>()->GetType();
				float temp = hit.object->GetComponent<PickupComponent>()->GetAmount();
				
				if (pickupType == Type::Health)
				{
					if ((health + temp) <= 100.0f)
						health += temp;
				}
				else if (pickupType == Type::Food)
				{
					if ((food + temp) <= 100.0f)
						food += temp;
				}
				else if (pickupType == Type::Fuel)
				{
					if ((fuel + temp) <= 100.0f)
						fuel += temp;
				}

				hit.object->GetComponent<PickupComponent>()->SetActive(false);
			}
		}
	}
	else
	{
		//DShape::DrawSphere(ray.GetPoint(10.0f), 0.2f, { 1, 0, 1 });
	}


	//temp fix for wierd clock start at 
	if (GameClock::Instance().GetFrameTime() / 1000 < 5.f)
	{
		//loose fuel
		//std::cout << GameClock::Instance().GetFrameTime() / 1000;
		fuel -= GameClock::Instance().GetFrameTime() / 1000 * fuelBurnPerMeter;
		//std::cout << fuel <<std::endl;

		// loose food
		food -= GameClock::Instance().GetFrameTime() / 1000 * foodLossPerSecond;
		//std::cout << food<<std::endl;
		
		// make better later
		if (fuel < 0 || health < 0 && !immortal)
			swapScene = NEXT_SCENE::LOSE;

		if (food < 0)
			foodEmpty = true;

		if (foodEmpty)
		{
			health -= GameClock::Instance().GetFrameTime() / 1000 * healthLossPerSecond;
		}
	}
	
	// Fuel drop
	fuelDippingBar->SetScaleBars(ReverseAndClamp(fuel));

	// Food drop
	foodDippingBar->SetScaleBars(ReverseAndClamp(food));

	// Health drop
	healthDippingBar->SetScaleBars(ReverseAndClamp(health));

	// Health dipping when hit by enemy
	//healthDippingBar->SetScaleDipping(0);
}

float PlayerComp::ReverseAndClamp(float inputValue)
{
	return 1.0f - (inputValue / 100.0f);
}
