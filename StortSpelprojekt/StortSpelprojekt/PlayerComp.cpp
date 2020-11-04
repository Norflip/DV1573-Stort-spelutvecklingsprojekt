#include "stdafx.h"
#include "PlayerComp.h"
#include "WeaponComponent.h"
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
	//this->GetOwner()->GetComponen
	//weaponsList.push_back()
}

PlayerComp::~PlayerComp()
{

}

void PlayerComp::Update(const float& deltaTime)
{
	RayCast(deltaTime);




	float frameTime = FCAST(GameClock::Instance().GetFrameTime() / 1000.0);

	//temp fix for wierd clock start at 
	if (frameTime < 5.f)
	{
		//loose fuel
		fuel -= frameTime * fuelBurnPerMeter;
		// loose food
		food -= frameTime * foodLossPerSecond;
#if !immortal
		// make better later
		if ((fuel < 0 || health <= 0))
			swapScene = NEXT_SCENE::LOSE;
#endif
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
}

void PlayerComp::InsertWeapon(WeaponComponent* weapon, std::string name)
{
	this->weaponsList.insert(make_pair(name, weapon));
	//SETS CURRENT WEAPON TO NEWLY INSERTED WEAPON
	currentWeapon = weapon->GetOwner();
}

float PlayerComp::ReverseAndClamp(float inputValue)
{
	return 1.0f - (inputValue / 100.0f);
}

void PlayerComp::RayCast(const float& deltaTime)
{


	Ray ray = cam->MouseToRay(p.x, p.y);
	Physics& phy = Physics::Instance();
	//PICKUP OBJECTS
	if (KEY_DOWN(E))
	{
		
			if (phy.RaytestSingle(ray, rayDistance, hit, FilterGroups::PICKUPS))
			{
				if (hit.object != nullptr)
				{
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
				rp::RigidBody* objectRb = rbComp->GetRigidBody();
				rbComp->RemoveCollidersFromBody(objectRb);
			}
			}
			if (phy.RaytestSingle(ray, rayDistance, hit, FilterGroups::HOLDABLE))
			{
				if (hit.object != nullptr)
				{
					hit.object->RemoveFlag(ObjectFlag::ENABLED);
					currentWeapon->RemoveFlag(ObjectFlag::ENABLED);
				}
			}

	}
	//ATTACK ENEMIES
	if (LMOUSE_DOWN)
	{
		if (phy.RaytestSingle(ray, 5.0f, hit, FilterGroups::ENEMIES))
		{
			if (hit.object != nullptr && hit.object->HasComponent<EnemyStatsComp>())
			{
				if (hit.object->GetComponent<EnemyStatsComp>()->IsEnabled())
				{
					if (hit.object->GetComponent<EnemyStatsComp>()->GetHealth() >= 0.0f)
					{
						hit.object->GetComponent<EnemyStatsComp>()->LoseHealth(attack);
						std::cout << "Hit hit hit" << std::endl;
					}
					else if (hit.object->GetComponent<EnemyStatsComp>()->GetHealth() <= 0.0f)
					{
						RigidBodyComponent* rbComp = hit.object->GetComponent<RigidBodyComponent>();
						rp::RigidBody* objectRb = rbComp->GetRigidBody();
						rbComp->RemoveCollidersFromBody(objectRb);
						hit.object->RemoveFlag(ObjectFlag::ENABLED);
						hit.object->AddFlag(ObjectFlag::REMOVED);
					}
				}
			}
		}
	}

}
