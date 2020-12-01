#include "stdafx.h"
#include "ControllerComp.h"
#include "Engine.h"

void ControllerComp::CheckGrounded()
{
	dx::XMFLOAT3 origin;
	dx::XMStoreFloat3(&origin, GetOwner()->GetTransform().GetPosition());
	//origin.z += 2.f;
	Ray ray(origin, DOWN_VEC);
	RayHit hitTerrain;
	//RayHit hitProps;

	//TERRAIN or default depending on if u can jump from on top of objects
	float distance = 1.45f;
	Physics* phy = Engine::Instance->GetPhysics();

	phy->RaytestSingle(ray, distance, hitTerrain, FilterGroups::TERRAIN | FilterGroups::PROPS);
	//phy->RaytestSingle(ray, distance, hitProps, FilterGroups::PROPS);
	
	
	this->isGrounded = false;
	if (hitTerrain.object != nullptr) //(hitProps.object != nullptr && hitProps.object->GetName() == "HouseInterior"))// != nullptr )//&& hitProps.object->GetName() == "houseBase"))
	{
		//this->houseVelocity = { 0.f,0.f,0.f };
		/*if (hitProps.object != nullptr && hitProps.object->GetName() == "houseBase")
		{
			dx::XMFLOAT3 move = hitProps.object->GetComponent<NodeWalkerComp>()->GetMoveVec();
			this->houseVelocity = move;
			dx::XMVECTOR houseVec = dx::XMLoadFloat3(&this->houseVelocity);
			houseVec = dx::XMVector3Normalize(houseVec);
			dx::XMStoreFloat3(&this->houseVelocity, houseVec);
			GetOwner()->GetTransform().Translate(move.x, move.y, move.z); 
			rbComp->SetPosition(GetOwner()->GetTransform().GetPosition());
		}*/

		std::cout << hitTerrain.object->GetName() << std::endl;

		this->isGrounded = true;
		//std::cout << "picking: " << hit.object->GetName() << std::endl;
		//DShape::DrawLine(ray.origin, ray.GetPoint(distance), { 0,0,1 });
	}
	
	//else
	//	DShape::DrawLine(ray.origin, ray.GetPoint(distance), { 1,0,0 });
	
	//return result;
}

ControllerComp::ControllerComp(Object* cameraObject, Object* houseObject)
{
	//this->fov = 60.f;
	//this->fovTimer = 0.f;

	this->velocity = 0.f;
	this->velocityTimer = 0.f;
	this->crouchTimer = 0.f;

	this->freeCam = false;
	this->showCursor = false;
	this->canRotate = true;
	this->isGrounded = false;
	this->houseVelocity = { 0.f,0.f,0.f };
	this->jumpDir = { 0.f,0.f,0.f };
	this->cameraOffset = { 0.f,0.f,0.f };
	this->cameraEuler = { 0.f,45.f,0.f }; //TODO_: fix start angle (looks down or up at start)

	this->cameraObject = cameraObject;
	this->houseWalkComp = houseObject->GetComponent<NodeWalkerComp>();
	this->rbComp = nullptr;
	this->camComp = nullptr;
	this->capsuleComp = nullptr;
	this->playerComp = nullptr;
	this->inside = false;
	this->inDoorRange = false;
	this->first = true;
}

ControllerComp::~ControllerComp()
{
}

void ControllerComp::Initialize()
{
	this->rbComp = GetOwner()->GetComponent<RigidBodyComponent>();
	rbComp->m_InitializeBody(Engine::Instance->GetPhysics());

	this->camComp = cameraObject->GetComponent<CameraComponent>();
	this->capsuleComp = GetOwner()->GetComponent<CapsuleColliderComponent>();
	this->playerComp = GetOwner()->GetComponent<PlayerComp>();
	ShowCursor(!this->canRotate);

	if (this->canRotate)
		Input::Instance().SetMouseMode(dx::Mouse::MODE_RELATIVE);
	else
		Input::Instance().SetMouseMode(dx::Mouse::MODE_ABSOLUTE);

	this->camComp->SetFOV(WALK_FOV);
	this->rbComp->LockRotation(true);
	//this->rbComp->SetLinearDamping(0.3f);
	this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setBounciness(0.f);
	this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setRollingResistance(10.f);
	this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setFrictionCoefficient(10.f);
	this->rbComp->EnableGravity(!this->freeCam);
	this->rbComp->SetLinearVelocity({ 0.f, 0.f, 0.f });

	dx::XMVECTOR reset = dx::XMLoadFloat4(&RESET_ROT);
	this->cameraObject->GetTransform().SetRotation(reset);
}


void ControllerComp::Update(const float& deltaTime)
{
	//WASD = move
	//space = jump 
	//0 = reset position rotation and xClamp
	//fov decrease when standing still or snap, velcity snap when no wasd
	//TODO: house radius and player length start/stop

	Physics* phy = Engine::Instance->GetPhysics();

	DirectX::XMFLOAT3 dir = { 0.f,0.f,0.f };
	//this->fovTimer += deltaTime;
	this->velocityTimer += deltaTime; 
	this->crouchTimer += deltaTime;

	if (KEY_DOWN(D0))
	{
		this->cameraObject->GetTransform().SetRotation(dx::XMLoadFloat4(&RESET_ROT));
		this->cameraOffset = { 0.f,0.f,0.f };
		this->cameraEuler = { 0.f, 0.f, 0.f };
		this->jumpDir = { 0.f, 0.f, 0.f };

		rbComp->SetPosition(dx::XMLoadFloat3(&RESET_POS));
		rbComp->SetLinearVelocity({ 0.f, 0.f, 0.f });
	}

	if (KEY_DOWN(O))
	{
		this->showCursor = !this->showCursor;
		ShowCursor(this->showCursor);
	}

	if (KEY_DOWN(V))
	{
		this->freeCam = !this->freeCam;
		
		rbComp->SetLinearVelocity({ 0.f, 0.f, 0.f });
		rbComp->EnableGravity(!this->freeCam);
	}

	if (KEY_DOWN(F))
	{
		this->canRotate = !this->canRotate;
		rbComp->SetLinearVelocity({ 0.f, 0.f, 0.f });
		ShowCursor(!this->canRotate);
		if (this->canRotate)
			Input::Instance().SetMouseMode(dx::Mouse::MODE_RELATIVE);
		else
			Input::Instance().SetMouseMode(dx::Mouse::MODE_ABSOLUTE);
	}
	
	float length = 0.f;
	dx::XMVECTOR lengthVec;
	if (!inside)
	{
		// Check length from player
		lengthVec = dx::XMVector3Length(dx::XMVectorSubtract(houseWalkComp->GetOwner()->GetTransform().GetPosition(), GetOwner()->GetTransform().GetPosition()));
		dx::XMStoreFloat(&length, lengthVec);
	}
	
	if (houseWalkComp->GetIsWalking())
	{
		// If next to the house
		if (length > playerComp->GetRadius() || length < 7.0f)
			houseWalkComp->Stop();
	}
	else if (!houseWalkComp->GetIsWalking())
	{
		if (length < playerComp->GetRadius() && length > 7.0f && !inside)
			houseWalkComp->Start();

		if (RMOUSE_DOWN)
		{
			if (inside && inDoorRange)
			{
				if (first)
				{
					dx::XMFLOAT3 pos = this->playerComp->GetStartPosition();
					GetOwner()->GetTransform().SetPosition(dx::XMVECTOR{ pos.x, pos.y, pos.z, 0 });
					rbComp->SetPosition(dx::XMVECTOR{ pos.x, pos.y, pos.z, 0 });
					inside = false;
					first = false;
				}
				else
				{
					GetOwner()->GetTransform().SetPosition(dx::XMVECTOR{ this->outsidePos.x, this->outsidePos.y, this->outsidePos.z, 0 });
					rbComp->SetPosition(dx::XMVECTOR{ this->outsidePos.x, this->outsidePos.y, this->outsidePos.z, 0 });
					inside = false;
				}
			}
			else if (inDoorRange && !inside)
			{
				dx::XMVECTOR current = rbComp->GetPosition();

				this->outsidePos = { current.m128_f32[0], current.m128_f32[1], current.m128_f32[2] };
				dx::XMFLOAT3 interior = this->playerComp->GetInteriorPosition();

				GetOwner()->GetTransform().SetPosition(dx::XMVECTOR{ interior.x, interior.y + 3.0f, interior.z, 0 });
				rbComp->SetPosition({ interior.x, interior.y + 3.0f, interior.z, 0.0f });

				inside = true;
				inDoorRange = false;
			}
		}
	}
	
	if (!inside)	
		AudioMaster::Instance().PlayerOutside(true);	
	else	
		AudioMaster::Instance().PlayerOutside(false);
	
	if (!playerComp->GetIsReading())
	{
		if (this->canRotate)
		{
			//Input::Instance().ConfineMouse();
			//SetCursorPos(400, 400); //set this to coordinates middle of screen? get height/width from input?

			float xPos = Input::Instance().GetMousePosRelative().x * deltaTime;
			float yPos = Input::Instance().GetMousePosRelative().y * deltaTime;
			cameraEuler.x += xPos;
			cameraEuler.y += yPos;
			cameraEuler2.x += xPos;
			cameraEuler2.y += yPos;

			cameraEuler.x = Math::Clamp(cameraEuler.x, -CLAMP_X, CLAMP_X);
			if (cameraEuler.y >= CLAMP_Y)
				cameraEuler.y = -CLAMP_Y;
			else if (cameraEuler.y <= -CLAMP_Y)
				cameraEuler.y = CLAMP_Y;

			//std::cout << "x: " << cameraEuler.x * Math::ToDegree << ", y: " << cameraEuler.y * Math::ToDegree << std::endl;
			cameraObject->GetTransform().SetRotation(dx::XMQuaternionRotationRollPitchYaw(cameraEuler.x, cameraEuler.y, cameraEuler.z));

			groundRotation = dx::XMQuaternionRotationRollPitchYaw(0.f, cameraEuler.y, 0.f);

			Input::Instance().ResetRelative();

			//MoveState isMoving = IDLE;

			/*if (isMoving == MoveState::IDLE)
				AudioMaster::Instance().StopSoundEvent("walk");*/

			if (KEY_PRESSED(W) || KEY_PRESSED(S) || KEY_PRESSED(A) || KEY_PRESSED(D))
			{
				isMoving = MoveState::WALKING;

				if (KEY_PRESSED(LeftShift)) //sprint
				{
					isMoving = MoveState::SPRINTING;
				}
				else
				{
					isMoving = MoveState::WALKING;
				}


				if (KEY_PRESSED(W))
					dir.z += 1.f;
				if (KEY_PRESSED(S))
					dir.z -= 1.f;
				if (KEY_PRESSED(A))
					dir.x -= 1.f;
				if (KEY_PRESSED(D))
					dir.x += 1.f;

			}
			else
			{
				isMoving = MoveState::IDLE;
			}


			if (freeCam) //flying camera
			{
				if (KEY_PRESSED(Space)) //Free cam
					dir.y += 1.f;
				if (KEY_PRESSED(C))
					dir.y -= 1.f;

				dx::XMVECTOR direction = dx::XMLoadFloat3(&dir);
				direction = dx::XMVector3Normalize(direction);

				//phy.MutexLock();
				direction = cameraObject->GetTransform().TransformDirection(direction);
				direction = dx::XMVectorScale(direction, RUN_VELOCITY);
				dx::XMStoreFloat3(&dir, direction);
				dx::XMFLOAT3 vel = rbComp->GetLinearVelocity();
				cameraObject->GetTransform().SetPosition(rbComp->GetPosition());
				rbComp->SetLinearVelocity({ dir.x , dir.y , dir.z });
				dx::XMVECTOR capsule = dx::XMLoadFloat4(&RESET_ROT);
				rbComp->SetRotation(capsule);
				//phy.MutexUnlock();
			}
			else //First Person specific actions
			{
				float jumpVelocity = 0;
				CheckGrounded();
				if (isGrounded && KEY_DOWN(Space)) // FPcam //jump is  scuffed
				{
					jumpVelocity = JUMP_VELOCITY;
					this->jumpDir.x = dir.x;
					this->jumpDir.z = dir.z;
				}

				if (KEY_PRESSED(LeftControl)) //crouch is scuffed and outdated
				{
					isMoving = CROUCHING;
				}

				//std::cout << this->cameraOffset.y << std::endl;
				float acceleration = 0.f;
				if (isMoving == IDLE)
				{
					if (this->velocity > 0.f) //is more decrease
						acceleration = -WALK_ACCELERATION;
					if (!inside)
					{
						AudioMaster::Instance().StopSoundEvent("insideWalk");
						AudioMaster::Instance().StopSoundEvent("walk");
						AudioMaster::Instance().StopSoundEvent("run");
					}
					else
					{
						AudioMaster::Instance().StopSoundEvent("insideWalk");
					}
				}
				else if (isMoving == WALKING)
				{
					if (this->velocity + WALK_ACCELERATION < WALK_VELOCITY) //is less increase
						acceleration = WALK_ACCELERATION;
					else if (this->velocity > WALK_VELOCITY) //is more decrease
						acceleration = -WALK_ACCELERATION;

					// Kan lägga in ljud för att gå på plankor här med, ha en bool för "onHouse" t.ex.
					if (isGrounded)
					{
						if (!inside)
						{
							AudioMaster::Instance().StopSoundEvent("insideWalk");
							AudioMaster::Instance().StopSoundEvent("run");
							AudioMaster::Instance().PlaySoundEvent("walk");
						}
						else
						{
							AudioMaster::Instance().PlaySoundEvent("insideWalk");
						}
					}
				}
				else if (isMoving == SPRINTING)
				{
					if (!inside)
					{
						if (this->velocity + RUN_ACCELERATION < RUN_VELOCITY) //is less increase
							acceleration = RUN_ACCELERATION;

						else if (this->velocity > RUN_VELOCITY)//is more decrease
							acceleration = -RUN_ACCELERATION;

						if (isGrounded)
						{
							//if (!inside)
							//{
								AudioMaster::Instance().StopSoundEvent("insideWalk");
								AudioMaster::Instance().PlaySoundEvent("run");
								AudioMaster::Instance().StopSoundEvent("walk");
							//}

						}
					}
				}
				else if (isMoving == CROUCHING)
				{
					if (this->velocity + CROUCH_ACCELERATION < CROUCH_VELOCITY) //is less increase
						acceleration = CROUCH_ACCELERATION;

					else if (this->velocity > CROUCH_VELOCITY) //is more decrease
						acceleration = -CROUCH_ACCELERATION;


					if (this->cameraOffset.y > CROUCH_LIMIT && this->crouchTimer >= CROUCH_INC_RATE)
					{
						this->cameraOffset.y -= CROUCH_OFFSET_PER;
						this->crouchTimer = 0.f;
					}
				}
				if (isMoving != CROUCHING)
				{
					if (this->cameraOffset.y < 0.f && this->crouchTimer >= CROUCH_INC_RATE)
					{
						this->cameraOffset.y += CROUCH_OFFSET_PER;
						this->crouchTimer = 0.f;
					}
				}
				//std::cout << "camera offset.y: " << cameraOffset.y << std::endl;
				this->CalcVelocity(acceleration);

				//float percentVel = 0.f;
				//if (this->velocity > WALK_VELOCITY)
				//	percentVel = this->velocity / RUN_VELOCITY;
				////std::cout << "vel: " << this->velocity << ", fov: " << this->fov << std::endl;
				//this->fov = WALK_FOV + (percentVel * (RUN_FOV - WALK_FOV));// 70+()
				//camComp->SetFOV(this->fov);

				dx::XMVECTOR direction = dx::XMLoadFloat3(&dir);
				//direction = dx::XMVector3Normalize(direction);

				//phy.MutexLock();
				direction = cameraObject->GetTransform().TransformDirectionCustomRotation(direction, groundRotation);
				direction = dx::XMVector3Normalize(direction);
				direction = dx::XMVectorScale(direction, this->velocity);
				dx::XMStoreFloat3(&dir, direction);
				dx::XMFLOAT3 vel = rbComp->GetLinearVelocity();
				dx::XMVECTOR cameraPos = dx::XMLoadFloat3(&this->cameraOffset);
				cameraPos = dx::XMVectorAdd(cameraPos, rbComp->GetPosition());
				cameraObject->GetTransform().SetPosition(cameraPos); //add camera-offset
				if (isGrounded)
					rbComp->SetLinearVelocity({ dir.x, vel.y + jumpVelocity, dir.z });
				else
				{
					dx::XMVECTOR jumpVec = dx::XMLoadFloat3(&jumpDir);
					jumpVec = cameraObject->GetTransform().TransformDirectionCustomRotation(jumpVec, groundRotation);
					jumpVec = dx::XMVector3Normalize(jumpVec);
					jumpVec = dx::XMVectorScale(jumpVec, this->velocity);
					dx::XMFLOAT3 newDir;
					dx::XMStoreFloat3(&newDir, jumpVec);
					newDir.x = (newDir.x + dir.x) * 0.5f;
					newDir.z = (newDir.z + dir.z) * 0.5f;
					rbComp->SetLinearVelocity({ newDir.x + houseVelocity.x, vel.y + jumpVelocity, newDir.z + houseVelocity.z });
				}
				dx::XMVECTOR capsule = dx::XMLoadFloat4(&RESET_ROT);
				//capsuleComp->SetRotation(capsule);			
				rbComp->SetRotation(capsule);


			}
			//phy.MutexUnlock();
		}
		else
		{
			//phy.MutexLock();
			//	Input::Instance().FreeMouse();

			rbComp->SetLinearVelocity({ 0.f, 0.f, 0.f });
			dx::XMVECTOR capsule = dx::XMLoadFloat4(&RESET_ROT);
			rbComp->SetRotation(capsule);
			//phy.MutexUnlock();
		}
	}
	else
	{
		isMoving = MoveState::IDLE;

		float acceleration = 0.0f;
		float jumpVelocity = 0.0f;

		if (this->velocity > 0.f) //is more decrease
			acceleration = -WALK_ACCELERATION;

		this->CalcVelocity(acceleration);
		AudioMaster::Instance().StopSoundEvent("walk");
		AudioMaster::Instance().StopSoundEvent("run");
		AudioMaster::Instance().StopSoundEvent("insideWalk");

		dx::XMVECTOR direction = dx::XMLoadFloat3(&dir);
		//direction = dx::XMVector3Normalize(direction);

		//phy.MutexLock();
		direction = cameraObject->GetTransform().TransformDirectionCustomRotation(direction, groundRotation);
		direction = dx::XMVector3Normalize(direction);
		direction = dx::XMVectorScale(direction, this->velocity);
		dx::XMStoreFloat3(&dir, direction);
		dx::XMFLOAT3 vel = rbComp->GetLinearVelocity();
		dx::XMVECTOR cameraPos = dx::XMLoadFloat3(&this->cameraOffset);
		cameraPos = dx::XMVectorAdd(cameraPos, rbComp->GetPosition());
		cameraObject->GetTransform().SetPosition(cameraPos); //add camera-offset
		if (isGrounded)
			rbComp->SetLinearVelocity({ dir.x, vel.y + jumpVelocity, dir.z });
		else
		{
			dx::XMVECTOR jumpVec = dx::XMLoadFloat3(&jumpDir);
			jumpVec = cameraObject->GetTransform().TransformDirectionCustomRotation(jumpVec, groundRotation);
			jumpVec = dx::XMVector3Normalize(jumpVec);
			jumpVec = dx::XMVectorScale(jumpVec, this->velocity);
			dx::XMFLOAT3 newDir;
			dx::XMStoreFloat3(&newDir, jumpVec);
			newDir.x = (newDir.x + dir.x) * 0.5f;
			newDir.z = (newDir.z + dir.z) * 0.5f;
			rbComp->SetLinearVelocity({ newDir.x + houseVelocity.x, vel.y + jumpVelocity, newDir.z + houseVelocity.z });
		}
		dx::XMVECTOR capsule = dx::XMLoadFloat4(&RESET_ROT);
		//capsuleComp->SetRotation(capsule);			
		rbComp->SetRotation(capsule);
	}
	
	phy->MutexUnlock();

}
