#include "stdafx.h"
#include "PlayerComp.h"
#include "Engine.h"

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
	this->throwStrength = 50;

	Reset();

	this->fuelDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("fuelDippingBar"));
	this->foodDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("foodDippingBar"));
	this->healthDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("healthDippingBar"));



	//this->GetOwner()->GetComponent<CameraComponent>()
	p.x = renderer->GetOutputWindow()->GetWidth() / 2;
	p.y = renderer->GetOutputWindow()->GetHeight() / 2;

	cam = camComp;
	// per frame shit
	this->rayDistance = 2.0f;
	up = { 0.0f, 1.0f, 1.0f };
	holding = nullptr;
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
			Engine::Instance->SwitchScene(SceneIndex::GAME_OVER);
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

	if (holding != nullptr)
	{
		HoldObject();
		DropObject();
		//todo. Regrab last weapon
	}
}

void PlayerComp::HoldObject()
{

	inverseViewMatrix = dx::XMMatrixInverse(nullptr, cam->GetViewMatrix());
	wepOffTrans = dx::XMMatrixTranslation(0.3f, -0.4f, 0.8f);
	wepOffRot = dx::XMMatrixRotationAxis(up, dx::XMConvertToRadians(-40.0f));

	wepWorld = wepOffRot * wepOffTrans * inverseViewMatrix;
	dx::XMMatrixDecompose(&weaponScale, &weaponRot, &weaponPos, wepWorld);

	holding->GetTransform().SetPosition(weaponPos);
	holding->GetTransform().SetRotation(weaponRot);
	holding->GetTransform().SetScale(weaponScale);

}

void PlayerComp::DropObject()
{
	if (KEY_DOWN(T))
	{
		dx::XMVECTOR camRot = cam->GetOwner()->GetTransform().GetRotation();
		camRot = cam->GetOwner()->GetTransform().TransformDirectionCustomRotation({ 0,0,1 }, camRot);

		RigidBodyComponent* rbComp = holding->GetComponent<RigidBodyComponent>();
		rp::RigidBody* objectRb = rbComp->GetRigidBody();

		cam->GetOwner()->GetTransform().GetRotation();
		wepWorld = wepOffRot * wepOffTrans * inverseViewMatrix;
		dx::XMMatrixDecompose(&weaponScale, &weaponRot, &weaponPos, wepWorld);
		rbComp->SetEnabled(true);
		objectRb->setAngularVelocity({ 0,0,0 });
		rbComp->SetPosition(weaponPos);
		rbComp->SetRotation(weaponRot);
		float tossSpeed = throwStrength / rbComp->GetMass();
		objectRb->setLinearVelocity({ dx::XMVectorGetX(camRot) * tossSpeed ,  dx::XMVectorGetY(camRot) * tossSpeed,  dx::XMVectorGetZ(camRot) * tossSpeed });
		holding = nullptr;
		currentWeapon->AddFlag(ObjectFlag::ENABLED);
	}
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

		//HELVETE
		if (physics->RaytestSingle(ray, rayDistance, hit, FilterGroups::HOLDABLE))
		{
			if (hit.object != nullptr)
			{
				holding = hit.object;
				RigidBodyComponent* rbComp = hit.object->GetComponent<RigidBodyComponent>();
				rp::RigidBody* objectRb = rbComp->GetRigidBody();
				//rbComp->RemoveCollidersFromBody(objectRb);
				rbComp->SetEnabled(false);
				//hit.object->GetComponent<BoxColliderComponent>()->SetEnabled(false);
				hit.object->GetComponent<BoxColliderComponent>()->SetRotation(0, { 5, 5, 5, 5 });
				//hit.object->RemoveFlag(ObjectFlag::ENABLED);
				currentWeapon->RemoveFlag(ObjectFlag::ENABLED);
			}
		}

	}
	//ATTACK ENEMIES
	if (LMOUSE_DOWN && holding == nullptr)
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

						//phy.UnregisterRigidBody(hit.object->GetComponent<RigidBodyComponent>());
						//*/
						Engine::Instance->GetActiveScene()->RemoveObject(hit.object);
					}
				}
			}
		}
	}

	// Health drop
	healthDippingBar->SetScaleBars(ReverseAndClamp(health));

	// Health dipping when hit by enemy
	//healthDippingBar->SetScaleDipping(0);
}

void PlayerComp::Reset()
{
	// defaulting some shit
	this->foodLossPerSecond = 0.3f;
	this->food = 50.0f;
	this->fuelBurnPerMeter = 0.7f;
	this->fuel = 50.0f;
	this->healthLossPerSecond = 0.5f;

	this->health = 50.0f; // <------------------ DONT FORGET TO REMOVE THIS LATER!
	foodEmpty = false;
	gg = false;
}
