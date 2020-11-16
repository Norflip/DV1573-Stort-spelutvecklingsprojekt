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
	this->holdAngle = dx::SimpleMath::Vector3( 0.3f, -0.4f, 0.8f );
	Reset();

	this->fuelDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("fuelDippingBar"));
	this->foodDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("foodDippingBar"));
	this->healthDippingBar = static_cast<GUISprite*>(guiMan->GetGUIObject("healthDippingBar"));

	this->fuelBar = static_cast<GUISprite*>(guiMan->GetGUIObject("fuelBar"));
	this->foodBar = static_cast<GUISprite*>(guiMan->GetGUIObject("foodBar"));
	this->healthBar = static_cast<GUISprite*>(guiMan->GetGUIObject("healthBar"));



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
#if !IMMORTAL
		// make better later
		if ((fuel < 0 || health <= 0) && !IMMORTAL)
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
	fuelBar->SetScaleColor(ReverseAndClamp(fuel));

	// Food drop
	foodDippingBar->SetScaleBars(ReverseAndClamp(food));
	foodBar->SetScaleColor(ReverseAndClamp(food));

	// Health drop
	healthDippingBar->SetScaleBars(ReverseAndClamp(health));
	healthBar->SetScaleColor(ReverseAndClamp(health));

	if (holding != nullptr)
	{
		HoldObject();
		DropObject();
	}
}

void PlayerComp::HoldObject()
{
	inverseViewMatrix = dx::XMMatrixInverse(nullptr, cam->GetViewMatrix());
	wepOffTrans.Translation(holdAngle);
	wepOffRot = wepOffRot.CreateFromAxisAngle(up, dx::XMConvertToRadians(-40.0f));
	wepWorld = wepOffRot * wepOffTrans * inverseViewMatrix;
	holding->AddFlag(ObjectFlag::NO_CULL);
	//something here doesn work properly
	//GetOwner()->AddFlag(ObjectFlag::NO_CULL);
	wepWorld.Decompose(weaponScale, weaponRot, weaponPos);
	holding->GetTransform().SetPosition(weaponPos);
	holding->GetTransform().SetRotation(weaponRot);
	holding->GetTransform().SetScale(weaponScale);

}

void PlayerComp::DropObject()
{
	if (KEY_DOWN(T))
	{
		holding->RemoveFlag(ObjectFlag::NO_CULL);
		dx::XMVECTOR camRot = cam->GetOwner()->GetTransform().GetRotation();
		camRot = cam->GetOwner()->GetTransform().TransformDirectionCustomRotation({ 0,0,1 }, camRot);

		RigidBodyComponent* rbComp = holding->GetComponent<RigidBodyComponent>();
		rp::RigidBody* objectRb = rbComp->GetRigidBody();

		cam->GetOwner()->GetTransform().GetRotation();
		wepWorld = wepOffRot * wepOffTrans * inverseViewMatrix;
		wepWorld.Decompose(weaponScale, weaponRot, weaponPos);
		rbComp->SetEnabled(true);
		objectRb->setAngularVelocity({ 0,0,0 });
		rbComp->SetPosition(weaponPos);
		rbComp->SetRotation(weaponRot);
		//objectRb->setLinearVelocity({ dx::XMVectorGetX(camRot) * forceAmount ,  dx::XMVectorGetY(camRot) * forceAmount,  dx::XMVectorGetZ(camRot) * forceAmount });

		if (holding->HasComponent<ParticleSystemComponent>())
			if (!holding->GetComponent<ParticleSystemComponent>()->GetActive())
				holding->GetComponent<ParticleSystemComponent>()->SetActive(true);
						
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

	// Check fireplace 
	if (physics->RaytestSingle(ray, rayDistance, hit, FilterGroups::PICKUPS) && holding)
	{
		if (hit.object != nullptr)
		{
			if (KEY_DOWN(T))
			{
				std::cout << "Pressed F" << std::endl;

				if (hit.object != nullptr)
				{
					std::cout << "Fill up fuel" << std::endl;

					float refill = holding->GetComponent<PickupComponent>()->GetAmount();
					if ((fuel + refill) <= 100.0f)
						fuel += refill;

					if (holding->HasComponent<ParticleSystemComponent>())
						if (holding->GetComponent<ParticleSystemComponent>()->GetActive())
							holding->GetComponent<ParticleSystemComponent>()->SetActive(false);

					holding->GetComponent<PickupComponent>()->SetActive(false);
					holding = nullptr;
					currentWeapon->AddFlag(ObjectFlag::ENABLED);
				}
			}
		}
	}

	// Pick up health and food
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
				
				if (hit.object->HasComponent<ParticleSystemComponent>())
					if(hit.object->GetComponent<ParticleSystemComponent>()->GetActive())
						hit.object->GetComponent<ParticleSystemComponent>()->SetActive(false);
				

				hit.object->GetComponent<PickupComponent>()->SetActive(false);
				//RigidBodyComponent* rbComp = hit.object->GetComponent<RigidBodyComponent>();
				//rbComp->Release();
				
			}
		}

		// Check fuel
		if (physics->RaytestSingle(ray, rayDistance, hit, FilterGroups::HOLDABLE))
		{
			if (hit.object != nullptr)
			{
				AudioMaster::Instance().PlaySoundEvent("pickupFuel");
				holding = hit.object;
				RigidBodyComponent* rbComp = hit.object->GetComponent<RigidBodyComponent>();
				rp::RigidBody* objectRb = rbComp->GetRigidBody();
				//rbComp->RemoveCollidersFromBody(objectRb);
				rbComp->SetEnabled(false);
				//hit.object->GetComponent<BoxColliderComponent>()->SetEnabled(false);
				hit.object->GetComponent<BoxColliderComponent>()->SetRotation(0, { 5, 5, 5, 5 });
				//hit.object->RemoveFlag(ObjectFlag::ENABLED);
				currentWeapon->RemoveFlag(ObjectFlag::ENABLED);
				
				if (holding->HasComponent<ParticleSystemComponent>())
					if (holding->GetComponent<ParticleSystemComponent>()->GetActive())
						holding->GetComponent<ParticleSystemComponent>()->SetActive(false);
				
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
						//std::cout << "Hit hit hit" << std::endl;
						
					}
					else if (hit.object->GetComponent<EnemyStatsComp>()->GetHealth() <= 0.0f)
					{
						AudioMaster::Instance().PlaySoundEvent("punch");
						//hit.object.s  ->GetComponent<PickupComponent>()->SetActive(false);

						//RigidBodyComponent* rbComp = hit.object->GetComponent<RigidBodyComponent>();
						//rbComp->Release();

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
