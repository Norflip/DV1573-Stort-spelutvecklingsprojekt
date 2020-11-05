#include "stdafx.h"
#include "ControllerComp.h"

bool ControllerComp::IsGrounded() const
{
	dx::XMFLOAT3 origin;
	dx::XMStoreFloat3(&origin, GetOwner()->GetTransform().GetPosition());
	//origin.z += 2.f;
	Ray ray(origin, DOWN_VEC);
	RayHit hitTerrain;
	RayHit hitProps;

	//TERRAIN or default depending on if u can jump from on top of objects
	float distance = 1.45f;
	Physics& phy = Physics::Instance();
	phy.RaytestSingle(ray, distance, hitTerrain, FilterGroups::TERRAIN);
	phy.RaytestSingle(ray, distance, hitProps, FilterGroups::PROPS);
	
	bool result = false;
	if (hitTerrain.object != nullptr || (hitProps.object != nullptr && hitProps.object->GetName()== "houseBase"))
	{

		if (hitProps.object != nullptr && hitProps.object->GetName() == "houseBase")
		{

			dx::XMFLOAT3 housePos;
			dx::XMStoreFloat3(&housePos, hitProps.object->GetTransform().GetPosition());
			dx::XMFLOAT3 houseLastPos = hitProps.object->GetComponent<NodeWalkerComp>()->GetLastPos();

			dx::XMFLOAT3 delta = { housePos.x-houseLastPos.x ,housePos.y-houseLastPos.y, housePos.z-houseLastPos.z};

			GetOwner()->GetTransform().Translate(delta.x, delta.y, delta.z);
			rbComp->SetPosition(GetOwner()->GetTransform().GetPosition());
			//rbComp->SetLinearVelocity(delta);
		}
		
			

		result = true;
		//std::cout << "picking: " << hit.object->GetName() << std::endl;
		//DShape::DrawLine(ray.origin, ray.GetPoint(distance), { 0,0,1 });
		
	}
	//else
	//	DShape::DrawLine(ray.origin, ray.GetPoint(distance), { 1,0,0 });
	
	return result;
}

ControllerComp::ControllerComp(Object* cameraObject, Object* houseObject)
{
	this->fov = 60.f;
	this->fovTimer = 0.f;

	this->velocity = 0.f;
	this->velocityTimer = 0.f;
	this->crouchTimer = 0.f;

	this->freeCam = false;
	this->showCursor = false;
	this->canRotate = true;
	this->jumpDir = { 0.f,0.f,0.f };
	this->cameraOffset = { 0.f,0.f,0.f };
	this->cameraEuler = { 0.f,0.f,0.f }; //TODO_: fix start angle (looks down or up at start)

	this->cameraObject = cameraObject;
	this->houseObject = houseObject;
	this->rbComp = nullptr;
	this->camComp = nullptr;
	this->capsuleComp = nullptr;
}

ControllerComp::~ControllerComp()
{
}

void ControllerComp::Initialize()
{
	this->rbComp = GetOwner()->GetComponent<RigidBodyComponent>();
	this->camComp = cameraObject->GetComponent<CameraComponent>();
	this->capsuleComp = GetOwner()->GetComponent<CapsuleColliderComponent>();
	ShowCursor(!this->canRotate);

	if (this->canRotate)
		Input::Instance().SetMouseMode(dx::Mouse::MODE_RELATIVE);
	else
		Input::Instance().SetMouseMode(dx::Mouse::MODE_ABSOLUTE);

	if (this->cameraObject && this->rbComp && this->capsuleComp && this->camComp)
	{
		this->rbComp->LockRotation(true);
		//this->rbComp->SetLinearDamping(0.3f);
		this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setBounciness(0.f);
		this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setRollingResistance(10.f);
		this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setFrictionCoefficient(10.f);
		this->rbComp->EnableGravity(!this->freeCam);
		this->rbComp->SetLinearVelocity({ 0.f, 0.f, 0.f });
	}
	else 
	{
		std::cout << "component missing" << std::endl;
	}

	dx::XMVECTOR reset = dx::XMLoadFloat4(&RESET_ROT);
	this->cameraObject->GetTransform().SetRotation(reset);
}

void ControllerComp::Update(const float& deltaTime)
{
	//WASD = move
	//space = jump
	//9 = reset 
	//0 = reset position rotation and xClamp

	Physics& phy = Physics::Instance();
	DirectX::XMFLOAT3 dir = { 0.f,0.f,0.f };
	this->fovTimer += deltaTime;
	this->velocityTimer += deltaTime; 
	this->crouchTimer += deltaTime;

	phy.MutexLock();
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

	if (this->canRotate)
	{
		//Input::Instance().ConfineMouse();
		//SetCursorPos(400, 400); //set this to coordinates middle of screen? get height/width from input?
		
		float xPos = Input::Instance().GetMousePosRelative().x * deltaTime;
		float yPos = Input::Instance().GetMousePosRelative().y * deltaTime;
		cameraEuler.x += xPos;
		cameraEuler.y += yPos;
		cameraEuler.x = Math::Clamp(cameraEuler.x, CLAMP_X, -CLAMP_X);
		if (cameraEuler.y >= CLAMP_Y)
			cameraEuler.y = 0.f;
		else if (cameraEuler.y <= 0.f)
			cameraEuler.y = CLAMP_Y;
		//std::cout << "x: " << cameraEuler.x * Math::ToDegree << ", y: " << cameraEuler.y * Math::ToDegree << std::endl;
		cameraObject->GetTransform().SetRotation(dx::XMQuaternionRotationRollPitchYaw(cameraEuler.x, cameraEuler.y, cameraEuler.z));

		dx::XMVECTOR groundRotation = dx::XMQuaternionRotationRollPitchYaw(0.f, cameraEuler.y, 0.f);
		
		Input::Instance().ResetRelative();
		MoveState isMoving = IDLE;
		if (KEY_PRESSED(W) || KEY_PRESSED(S) || KEY_PRESSED(A) || KEY_PRESSED(D))
		{
			isMoving = WALKING;
			if (KEY_PRESSED(LeftShift)) //sprint
				isMoving = SPRINTING;
		
			if (KEY_PRESSED(W))
				dir.z += 1.f;
			if (KEY_PRESSED(S))
				dir.z -= 1.f;
			if (KEY_PRESSED(A))
				dir.x -= 1.f;
			if (KEY_PRESSED(D))
				dir.x += 1.f;
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
			if (IsGrounded() && KEY_DOWN(Space)) // FPcam //jump is  scuffed
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
				
			}
			else if (isMoving == WALKING)
			{
				if (this->velocity+ WALK_ACCELERATION < WALK_VELOCITY) //is less increase
					acceleration = WALK_ACCELERATION;
				else if(this->velocity > WALK_VELOCITY) //is more decrease
					acceleration = -WALK_ACCELERATION;
					
			}
			else if (isMoving == SPRINTING)
			{
				if (this->velocity + RUN_ACCELERATION < RUN_VELOCITY) //is less increase
					acceleration = RUN_ACCELERATION;
				
				else if (this->velocity > RUN_VELOCITY)//is more decrease
					acceleration = -RUN_ACCELERATION;

			}
			else if (isMoving==CROUCHING)
			{
				if (this->velocity+ CROUCH_ACCELERATION < CROUCH_VELOCITY) //is less increase
					acceleration = CROUCH_ACCELERATION;
				
				else if (this->velocity > CROUCH_VELOCITY) //is more decrease
					acceleration = -CROUCH_ACCELERATION;
				
			
				if (this->cameraOffset.y > CROUCH_LIMIT && this->crouchTimer >= CROUCH_INC_RATE)
				{
					this->cameraOffset.y -= CROUCH_OFFSET_PER;
					this->crouchTimer = 0.f;
				}
			}
			this->calcVelocity(acceleration);

			float percentVel = 0.f;
			if (this->velocity > WALK_VELOCITY)
				percentVel = this->velocity / RUN_VELOCITY;
			//std::cout << "vel: " << this->velocity << ", fov: " << this->fov << std::endl;
			this->fov = WALK_FOV + (percentVel * (RUN_FOV - WALK_FOV));// 70+()
			camComp->SetFOV(this->fov);
			
			dx::XMVECTOR direction = dx::XMLoadFloat3(&dir);
			//direction = dx::XMVector3Normalize(direction);

			//phy.MutexLock();
			direction = cameraObject->GetTransform().TransformDirectionCustomRotation(direction, groundRotation);
			direction = dx::XMVector3Normalize(direction);
			direction = dx::XMVectorScale(direction, this->velocity );
			dx::XMStoreFloat3(&dir, direction);
			dx::XMFLOAT3 vel = rbComp->GetLinearVelocity();
			dx::XMVECTOR cameraPos = dx::XMLoadFloat3(&this->cameraOffset);
			cameraPos = dx::XMVectorAdd(cameraPos, rbComp->GetPosition());
			cameraObject->GetTransform().SetPosition(cameraPos); //add camera-offset
			if(IsGrounded())
				rbComp->SetLinearVelocity({ dir.x, vel.y + jumpVelocity, dir.z});
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
				rbComp->SetLinearVelocity({ newDir.x, vel.y + jumpVelocity, newDir.z});
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
	phy.MutexUnlock();
#if NDEBUG 

	// fixes a bug in release where the compiler removes the variables
	// Later: fix these variables?

	//fov = 60.f;
	//fovTimer = 0.f;

	//velocity = 0.f;
	//velocityTimer = 0.f;
	//crouchTimer = 0.f;

	//xClamp = 0.f;
	//freeCam = true;
	//showCursor = true;
	//canRotate = false;

	//groundQuaterion = { 0.f,0.f,0.f,1.f };
	//cameraObject = nullptr;
	//rbComp = nullptr;
	//camComp = nullptr;
	//capsuleComp = nullptr;

#endif // NDEBUG 
}
