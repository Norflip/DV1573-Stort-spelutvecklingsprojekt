#include "stdafx.h"
#include "PlayerComp.h"
#include "Engine.h"

#include "WeaponComponent.h"
#include "EnemyManager.h"

constexpr float STILL_REDUCTION = 0.7f;

PlayerComp::PlayerComp(Renderer* renderer, CameraComponent* camComp, Object* house, Physics* physics, GUIManager* guimanager, float health, float movementSpeed, float radius, float attack, float attackSpeed)
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
	this->hpLossDist = 70;
	this->maxDist = 130;
	this->holdAngle = dx::SimpleMath::Vector3( 0.3f, -0.4f, 0.8f );
	this->house = house;
	this->hpLossPerDistance = 0.0001f;
	//Reset();

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
	up = { 0.0f, 1.0f, 1.0f };
	this->rayDistance = 2.0f;
	holding = nullptr;

	finishedTutorial = true; // Change to false if you wanna use the tutorial
	foodTutorial = false;
	fuelTutorial = false;
	healthTutorial = false;
	reading = false;
}

PlayerComp::~PlayerComp()
{

}

void PlayerComp::Update(const float& deltaTime)
{
	//float frameTime = FCAST(GameClock::Instance().GetFrameTime() / 1000.0);



	// Fuel drop

	fuelDippingBar->SetScaleBars(ReverseAndClamp(fuel));
	fuelBar->SetScaleColor(ReverseAndClamp(fuel));

	// Food drop
	foodDippingBar->SetScaleBars(ReverseAndClamp(food));
	foodBar->SetScaleColor(ReverseAndClamp(food));

	// Health drop
	healthDippingBar->SetScaleBars(ReverseAndClamp(health));
	healthBar->SetScaleColor(ReverseAndClamp(health));

	HoldObject();
	PickUpObject();
	DropObject();

	RayCast(deltaTime);

	if (!guiMan->GetGUIObject("door")->GetVisible() && !guiMan->GetGUIObject("fuel")->GetVisible()
		&& !guiMan->GetGUIObject("infoSprite")->GetVisible())
	{
		guiMan->GetGUIObject("dot")->SetVisible(true);
		guiMan->GetGUIObject("door")->SetVisible(false);
		guiMan->GetGUIObject("fuel")->SetVisible(false);
	}
	
	pickedUpLastFrame = false;

	if (!finishedTutorial)
		if (foodTutorial && fuelTutorial && healthTutorial)
			finishedTutorial = true;
}

void PlayerComp::FixedUpdate(const float& fixedDeltaTime)
{

	sm::Vector3 housePos = house->GetTransform().GetPosition();
	sm::Vector3 playerPos = this->GetOwner()->GetTransform().GetPosition();
	distance = playerPos.Distance(playerPos, housePos);

		//temp fix for wierd clock start at 
	//if (TARGET_FIXED_DELTA < 5.f)
	{


		//lose fuel if not inside house
		if (!GetOwner()->GetComponent<ControllerComp>()->GetInside() && fuel > 0.0f)
		{
			if (house->GetComponent<NodeWalkerComp>()->GetIsWalking())
				fuel -= (TARGET_FIXED_DELTA * fuelBurnPerMeter)* STILL_REDUCTION;

			else
				fuel -= (TARGET_FIXED_DELTA * fuelBurnPerMeter);

		}

		// lose food
		food -= TARGET_FIXED_DELTA * foodLossPerSecond;

	#if  !IMMORTAL
			if ((health <= 0))
				Engine::Instance->SwitchScene(SceneIndex::GAME_OVER);
	#endif //  !IMMORTAL

			if (food < 0)
				foodEmpty = true;

			if (foodEmpty)
			{
				health -= TARGET_FIXED_DELTA * healthLossPerSecond;
			}

			if (distance > hpLossDist && !GetOwner()->GetComponent<ControllerComp>()->GetInside())
				health -= distance * hpLossPerDistance;

			// around 90
			if (distance > maxDist && !GetOwner()->GetComponent<ControllerComp>()->GetInside())
				health = 0;
	}
}

void PlayerComp::HoldObject()
{
	if (holding != nullptr)
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
}

void PlayerComp::PickUpObject()
{
	if (holding == nullptr)
	{
		Ray ray = cam->MouseToRay(p.x, p.y);
		// Check fuel
		if (KEY_DOWN(E) && !pickedUpLastFrame)
		{
			if (physics->RaytestSingle(ray, rayDistance, hit, FilterGroups::HOLDABLE) && holding == nullptr)
			{
				if (hit.object != nullptr)
				{
					pickedUpLastFrame = true;
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
					arms->RemoveFlag(ObjectFlag::ENABLED);
					arms->GetComponent<PlayerAnimHandlerComp>()->SetEnabled(false);
					if (holding->HasComponent<ParticleSystemComponent>())
							holding->GetComponent<ParticleSystemComponent>()->SetActive(false);

				}
			}
		}
		// Pick up health and food
		if (KEY_DOWN(E) && !pickedUpLastFrame)
		{
			if (physics->RaytestSingle(ray, rayDistance, hit, FilterGroups::PICKUPS))
			{
				if (hit.object != nullptr)
				{
					AudioMaster::Instance().PlaySoundEvent("pickupSound");
					PickupType pickupType = hit.object->GetComponent<PickupComponent>()->GetType();
					float value = hit.object->GetComponent<PickupComponent>()->GetAmount();

					if (pickupType == PickupType::Health)
					{
						if ((health + value) <= 100.0f)
							health += value;
						else
							health = 100.0f;

						if (!healthTutorial)
							healthTutorial = true;
					}
					else if (pickupType == PickupType::Food)
					{
						if ((food + value) <= 100.0f)
							food += value;
						else
							food = 100.0f;

						if (!foodTutorial)
							foodTutorial = true;
					}
					/*else if (pickupType == PickupType::Fuel)
					{
						if ((fuel + value) <= 100.0f)
							fuel += value;
						else
							fuel = 100.0f;
					}*/

					if (hit.object->HasComponent<ParticleSystemComponent>())
							hit.object->GetComponent<ParticleSystemComponent>()->SetActive(false);

					hit.object->GetComponent<PickupComponent>()->SetActive(false);
					//RigidBodyComponent* rbComp = hit.object->GetComponent<RigidBodyComponent>();
					//rbComp->Release();
				}
			}
		}
	}
}

void PlayerComp::DropObject()
{
	if (holding != nullptr)
	{
		if (KEY_DOWN(E) && !pickedUpLastFrame)
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
					holding->GetComponent<ParticleSystemComponent>()->SetActive(true);

			float tossSpeed = throwStrength / rbComp->GetMass();
			objectRb->setLinearVelocity({ dx::XMVectorGetX(camRot) * tossSpeed ,  dx::XMVectorGetY(camRot) * tossSpeed,  dx::XMVectorGetZ(camRot) * tossSpeed });
			holding = nullptr;
			currentWeapon->AddFlag(ObjectFlag::ENABLED);
			arms->AddFlag(ObjectFlag::ENABLED);
			arms->GetComponent<PlayerAnimHandlerComp>()->SetEnabled(true);
		}
	}

}

void PlayerComp::InsertArms(Object* arms)
{
	this->arms = arms;
}

void PlayerComp::InsertWeapon(WeaponComponent* weapon, std::string name)
{
	this->weaponsList.insert(make_pair(name, weapon));
	//SETS CURRENT WEAPON TO NEWLY INSERTED WEAPON
	currentWeapon = weapon->GetOwner();
}

void PlayerComp::SetInteriorPosition(float x, float y, float z)
{
	this->interiorPosition = { x, y, z };
}

void PlayerComp::SetStartPosition(dx::XMVECTOR pos)
{
	this->startPos.x = dx::XMVectorGetX(pos);
	this->startPos.y = dx::XMVectorGetY(pos);
	this->startPos.z = dx::XMVectorGetZ(pos);
}

float PlayerComp::ReverseAndClamp(float inputValue)
{
	return 1.0f - (inputValue / 100.0f);
}

void PlayerComp::RayCast(const float& deltaTime)
{
	Ray ray = cam->MouseToRay(p.x, p.y);

	// Check fireplace 
	if (physics->RaytestSingle(ray, rayDistance, hit, FilterGroups::FIRE) && holding)
	{
		if (hit.object != nullptr)
		{
			guiMan->GetGUIObject("fuel")->SetVisible(true);
			guiMan->GetGUIObject("dot")->SetVisible(false);

			if (RMOUSE_DOWN)
			{
				float refill = holding->GetComponent<PickupComponent>()->GetAmount();
				if ((fuel + refill) <= 100.0f)
					fuel += refill;
				else
					fuel = 100.0f;

				if (holding->HasComponent<ParticleSystemComponent>())
						holding->GetComponent<ParticleSystemComponent>()->SetActive(false);

				holding->GetComponent<PickupComponent>()->SetActive(false);
				holding = nullptr;
				currentWeapon->AddFlag(ObjectFlag::ENABLED);

				arms->AddFlag(ObjectFlag::ENABLED);
				arms->GetComponent<PlayerAnimHandlerComp>()->SetEnabled(true);

				guiMan->GetGUIObject("fuel")->SetVisible(false);
				guiMan->GetGUIObject("dot")->SetVisible(true);

				if (!fuelTutorial)
					fuelTutorial = true;
			}
		}
		else
		{
			guiMan->GetGUIObject("fuel")->SetVisible(false);
		}
	}

	//KANSKE FPR LÄGGAS OM SEDAN
	// Check door
	if (physics->RaytestSingle(ray, rayDistance, hit, FilterGroups::DOOR))
	{
		if (finishedTutorial)
		{
			if (hit.object != nullptr)
			{
				this->GetOwner()->GetComponent<ControllerComp>()->SetInRange(true);
			}
			else
			{
				this->GetOwner()->GetComponent<ControllerComp>()->SetInRange(false);
			}
		}
		else
		{
			if (RMOUSE_DOWN)
			{
				if (hit.object != nullptr)
				{
					if (guiMan->GetGUIObject("infoSprite")->GetVisible())
					{
						guiMan->GetGUIObject("infoSprite")->SetVisible(false);
						guiMan->GetGUIObject("dot")->SetVisible(true);
						reading = false;
					}
					else
					{
						guiMan->GetGUIObject("infoSprite")->SetVisible(true);
						guiMan->GetGUIObject("dot")->SetVisible(false);
						reading = true;
					}
				}
			}
		}
	}

	//Click signs
	if (KEY_DOWN(E))
	{
		if (physics->RaytestSingle(ray, rayDistance, hit, FilterGroups::CLICKABLE))
		{
			if (hit.object != nullptr)
			{
				clickable = hit.object;
				RigidBodyComponent* rbComp = hit.object->GetComponent<RigidBodyComponent>();
				rp::RigidBody* objectRb = rbComp->GetRigidBody();
				hit.object->GetComponent<BoxColliderComponent>()->SetRotation(0, { 5, 5, 5, 5 });
		
				clickable->GetComponent<SelectableComponent>()->SetActive(true);
			}
		}
	}

	//ATTACK ENEMIES
	if (LMOUSE_DOWN && holding == nullptr && 
		arms->GetComponent< PlayerAnimHandlerComp>()->GetCooldown() > 1.0f)
	{
		if (physics->RaytestSingle(ray, 5.0f, hit, FilterGroups::ENEMIES))
		{
			if (hit.object != nullptr)
			{
				EnemyStatsComp* stats = hit.object->GetComponent<EnemyStatsComp>();
				SkeletonMeshComponent* skeleton = hit.object->GetComponent<SkeletonMeshComponent>();
				if (stats != nullptr && stats->IsEnabled() && stats->GetHealth() >= 0.0f)
				{
					stats->LoseHealth(attack);
					AudioMaster::Instance().PlaySoundEvent("punch");

					//if (stats->GetHealth() <= 0.0f)
					//{

					//	skeleton->SetTrack(SkeletonStateMachine::DEATH, true);
					//	stats->GetManager()->RemoveEnemy(hit.object);
					//	

					//		/*RigidBodyComponent* rbComp = hit.object->GetComponent<RigidBodyComponent>();
					//		rbComp->Release();
					//		Engine::Instance->GetActiveScene()->RemoveObject(hit.object);*/
					//}
				}
			}
		}

		/*else if (physics->RaytestSingle(ray, 5.0f, hit, FilterGroups::PROPS))
		{
			AudioMaster::Instance().PlaySoundEvent("choptree");			
		}*/
	}

	// Health drop
	healthDippingBar->SetScaleBars(ReverseAndClamp(health));

	// Health dipping when hit by enemy
	//healthDippingBar->SetScaleDipping(0);
}

float PlayerComp::GetDangerDistance() 
{
	//IF WE ARE INSIDE
	if (GetOwner()->GetComponent<ControllerComp>()->GetInside())
		return 0;

	if (distance > hpLossDist)
		return distance;
	else
		return 1;
}

void PlayerComp::SetStatsFromState(const SaveState& state)
{
	this->food = state.playerFood;
	this->fuel = state.playerFuel;
	this->health = state.playerHealth;

	// defaulting some shit
	this->foodLossPerSecond = 0.3f;
	this->fuelBurnPerMeter = 0.5f;
	this->healthLossPerSecond = 0.5f;
	this->holding = nullptr;
	this->foodEmpty = false;
}


