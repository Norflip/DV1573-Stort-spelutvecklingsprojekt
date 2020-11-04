#include "stdafx.h"
#include "PlayerComp.h"
#include "Engine.h"

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

PlayerComp::PlayerComp(Renderer* renderer, CameraComponent* camComp, Physics* physics, GUIManager* guimanager, float health, float movementSpeed, float radius, float attack, float attackSpeed)
{
	//attackTimer.Start();

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
	this->radius = radius;
	this->renderer = renderer;
	this->physics = physics;

	//this->enemyStatsComp = enemyComp->GetComponent<EnemyStatsComp>();

	// defaulting some shit
	this->swapScene = NEXT_SCENE::GAME;
	this->foodLossPerSecond = 0.3f;
	this->food = 50.0f;
	this->fuelBurnPerMeter = 0.7f;
	this->fuel = 50.0f;
	this->healthLossPerSecond = 0.5f;

	this->health = 50.0f; // <------------------ DONT FORGET TO REMOVE THIS LATER!

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
	this->rayDistance = 2.0f;
	
}

PlayerComp::~PlayerComp()
{

}

void PlayerComp::Update(const float& deltaTime)
{	
	//attackTimer.Update();
	Ray ray = cam->MouseToRay(p.x, p.y);

	if (KEY_DOWN(E))
	{				
		if (physics->RaytestSingle(ray, rayDistance, hit, FilterGroups::PICKUPS))
		{
			
			if (hit.object != nullptr)
			{	
				AudioMaster::Instance().PlaySoundEvent("pickupSound");
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

				RigidBodyComponent* rbComp = hit.object->GetComponent<RigidBodyComponent>();
				rbComp->Release();
				
				//phy.MutexLock();
				//phy.UnregisterRigidBody(hit.object->GetComponent<RigidBodyComponent>());
				//phy.MutexUnlock();
				
			}
		}
	}
	else
	{
		//DShape::DrawSphere(ray.GetPoint(10.0f), 0.2f, { 1, 0, 1 });
	}

	if (LMOUSE_DOWN)
	{
		
		if (physics->RaytestSingle(ray, 5.0f, hit, FilterGroups::ENEMIES))
		{
			if (hit.object != nullptr && hit.object->HasComponent<EnemyStatsComp>())
			{				
				if (hit.object->GetComponent<EnemyStatsComp>()->IsEnabled())
				{
					if (hit.object->GetComponent<EnemyStatsComp>()->GetHealth() >= 0.0f)
					{				
						AudioMaster::Instance().PlaySoundEvent("punch");
						hit.object->GetComponent<EnemyStatsComp>()->LoseHealth(attack);
						std::cout << "Hit hit hit" << std::endl;
					}
					else if (hit.object->GetComponent<EnemyStatsComp>()->GetHealth() <= 0.0f)
					{
						AudioMaster::Instance().PlaySoundEvent("punch");
						//hit.object.s  ->GetComponent<PickupComponent>()->SetActive(false);
						
						RigidBodyComponent* rbComp = hit.object->GetComponent<RigidBodyComponent>();
						rbComp->Release();

						////hit.object->GetComponent<EnemyStatsComp>()->SetEnabled(false);
						////hit.object->GetComponent<EnemyStatsComp>()->SetEnabled(false);
						///*hit.object->GetComponent<EnemyStatsComp>()->SetEnabled(false);
						//RigidBodyComponent* rbComp = hit.object->GetComponent<RigidBodyComponent>();
						//rp::RigidBody* objectRb = rbComp->GetRigidBody();
						//rbComp->RemoveCollidersFromBody(objectRb);
						//phy.UnregisterRigidBody(hit.object->GetComponent<RigidBodyComponent>());
						//*/
						Engine::Instance->GetActiveScene()->RemoveObject(hit.object);
					}
				}
				else
				{
					std::cout << "YADDA" << std::endl;
				}
			}
			else
			{
				std::cout << " jadda hallå " << std::endl;
			}
		}
		else
		{

		}
	}
	else
	{

	}

	float frameTime = FCAST(GameClock::Instance().GetFrameTime() / 1000.0);

	//temp fix for wierd clock start at 
	if (frameTime < 5.f)
	{
		//loose fuel
		//std::cout << GameClock::Instance().GetFrameTime() / 1000;
		fuel -= frameTime * fuelBurnPerMeter;
		//std::cout << fuel <<std::endl;

		// loose food
		food -= frameTime * foodLossPerSecond;
		//std::cout << food<<std::endl;
		
		// make better later
		if (fuel < 0 || health <= 0 && !immortal)
			swapScene = NEXT_SCENE::LOSE;

		if (food < 0)
			foodEmpty = true;

		if (foodEmpty)
		{
			health -= frameTime * healthLossPerSecond;
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
