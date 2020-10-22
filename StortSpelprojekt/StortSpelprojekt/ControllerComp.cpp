#include "ControllerComp.h"

ControllerComp::ControllerComp()
{
	this->velocity = 0.f;
	this->velocityTimer = 0.f;

	this->xClamp = 0.f;
	this->freeCam = true;
	this->showCursor = true;
	this->canRotate = false;
	this->fov = 60.f;
	this->fovTimer = 0.f;
	
	this->groundQuaterion = { 0.f,0.f,0.f,1.f };
	this->cameraObject = nullptr;
	this->rbComp = nullptr;
	this->camComp = nullptr;
	this->capsuleComp = nullptr;
}

ControllerComp::ControllerComp(Object* cameraObject)
{
	this->velocity = 0.f;
	this->velocityTimer = 0.f;

	this->xClamp = 0.f;
	this->freeCam = true;
	this->showCursor = true;
	this->canRotate = false;
	this->fov = 60.f;
	this->fovTimer = 0.f;

	this->groundQuaterion = { 0.f,0.f,0.f,1.f };
	this->cameraObject = cameraObject;
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

	if (this->cameraObject && this->rbComp && this->capsuleComp && this->camComp)
	{
		this->rbComp->LockRotation(true);
		this->rbComp->SetLinearDamping(0.1f);
		this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setBounciness(0.f);
		this->rbComp->EnableGravity(!this->freeCam);
		this->rbComp->SetLinearVelocity({ 0.f, 0.f, 0.f });
		//this->capsuleComp->SetEnabled(true);
	}
	else 
	{
		std::cout << "component missing" << std::endl;
	}
}

void ControllerComp::Update(const float& deltaTime)
{
	Physics& phy = Physics::Instance();
	DirectX::XMFLOAT3 dir = { 0.f,0.f,0.f };
	this->fovTimer += deltaTime;
	this->velocityTimer += deltaTime;
	dx::XMVECTOR groundRotation;

	if (KEY_DOWN(D0))
	{
		dx::XMVECTOR resetPos = dx::XMLoadFloat3(&RESET_POS);
		GetOwner()->GetTransform().SetPosition(resetPos);
		rbComp->SetPosition(resetPos);
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
		xClamp += xPos;
		if (xClamp > CLAMP_X)
		{
			xClamp = CLAMP_X;
			xPos = 0.f;
		}
		if (xClamp < -CLAMP_X)
		{
			xClamp = -CLAMP_X;
			xPos = 0.f;
		}

		//rotate view
		Transform& transform = cameraObject->GetTransform();
		dx::XMVECTOR right = transform.TransformDirection({ 1,0,0 });
		dx::XMVECTOR eulerRotation = dx::XMQuaternionMultiply(dx::XMQuaternionRotationAxis(right, xPos), dx::XMQuaternionRotationAxis({ 0,1,0 }, yPos));

		//rotate walking direction to allign to ground
		groundRotation = dx::XMQuaternionMultiply(dx::XMQuaternionRotationAxis(right, 0), dx::XMQuaternionRotationAxis({ 0,1,0 }, yPos));
		groundRotation = dx::XMQuaternionMultiply(dx::XMLoadFloat4(&this->groundQuaterion), groundRotation);
		dx::XMStoreFloat4(&groundQuaterion, groundRotation); 

		transform.SetRotation(dx::XMQuaternionMultiply(transform.GetRotation(), eulerRotation)); //rotate view
		Input::Instance().ResetRelative();

		if (KEY_PRESSED(LeftShift)) //sprint
		{
			if (this->velocity < RUN_VELOCITY && this->velocityTimer >= VELOCITY_INC_RATE)
			{
				this->velocity += RUN_ACCELERATION;
				this->velocityTimer = 0.f;
			}
			if (this->fov < RUN_FOV && this->fovTimer >= FOV_INC_RATE)
			{
				this->fov += FOV_INC;
				this->fovTimer = 0.f;
			}
		}
		else
		{
			if (this->velocity > WALK_VELOCITY && this->velocityTimer >= VELOCITY_INC_RATE)
			{
				this->velocity -= RUN_ACCELERATION;
				this->velocityTimer = 0.f;
			}
			if (this->fov > WALK_FOV && this->fovTimer >= FOV_INC_RATE)
			{
				this->fov -= FOV_INC;
				this->fovTimer = 0.f;
			}
		}
		
		camComp->SetFOV(fov);

		if (KEY_PRESSED(W) || KEY_PRESSED(S) || KEY_PRESSED(A) || KEY_PRESSED(D))
		{
			if (KEY_PRESSED(W))
				dir.z += 1.f;
			if (KEY_PRESSED(S))
				dir.z -= 1.f;
			if (KEY_PRESSED(A))
				dir.x -= 1.f;
			if (KEY_PRESSED(D))
				dir.x += 1.f;

			if (this->velocity < WALK_VELOCITY && velocityTimer >= VELOCITY_INC_RATE)
			{
				this->velocity += WALK_ACCELERATION;
				this->velocityTimer = 0.f;
			}
		}
		else
		{
			if (this->velocity > 0.f && velocityTimer >= VELOCITY_INC_RATE)
			{
				this->velocity -= WALK_ACCELERATION;
				this->velocityTimer = 0.f;
			}
		}

		if (freeCam) //flying camera
		{
			if (KEY_PRESSED(Space)) //Free cam
				dir.y += 1.f;
			if (KEY_PRESSED(C))
				dir.y -= 1.f;
		}
		else //First Person specific actions
		{

			//if (rbComp->IsGrounded())
			//{
			//	//std::cout << "hit ground" << std::endl;
			//}
			if (KEY_PRESSED(Space)) // FPcam //jump is  scuffed
			{
				phy.MutexLock();
					rbComp->AddForce({ 0.f, 600.f, 0.f });
				phy.MutexUnlock();
			}

			//if (KEY_DOWN(LeftControl)) //crouch is scuffed and outdated
			//{
			//	if (this->velocity > CROUCH_VELOCITY && velocityTimer >= VELOCITY_INC_RATE)
			//	{
			//		this->velocity -= CROUCH_ACCELERATION;
			//		this->velocityTimer = 0.f;
			//	}
			//	GetOwner()->GetTransform().Translate(-CROUCH.x, -CROUCH.y, -CROUCH.z);
			//}
			//if (KEY_UP(LeftControl))
			//{
			//	if (this->velocity < WALK_VELOCITY && velocityTimer >= VELOCITY_INC_RATE)
			//	{
			//		this->velocity += CROUCH_ACCELERATION;
			//		this->velocityTimer = 0.f;
			//	}
			//	GetOwner()->GetTransform().Translate(CROUCH.x, CROUCH.y, CROUCH.z);
			//}
		}

		dx::XMVECTOR direction = dx::XMLoadFloat3(&dir);
		direction = dx::XMVector3Normalize(direction);
		if (freeCam)
		{
			phy.MutexLock();
			direction = cameraObject->GetTransform().TransformDirection(direction);
			direction = dx::XMVectorScale(direction, RUN_VELOCITY * deltaTime);
			dx::XMStoreFloat3(&dir, direction);
			dx::XMFLOAT3 vel = rbComp->GetLinearVelocity();
			cameraObject->GetTransform().SetPosition(rbComp->GetPosition()); 
			rbComp->SetLinearVelocity({ dir.x * VELOCITY_MULTIPLIER, dir.y* VELOCITY_MULTIPLIER, dir.z * VELOCITY_MULTIPLIER });
			dx::XMVECTOR capsule = dx::XMLoadFloat4(&RESET_ROT);
			rbComp->SetRotation(capsule);
			phy.MutexUnlock();
		}
		else
		{
			phy.MutexLock();
			direction = cameraObject->GetTransform().TransformDirectionCustomRotation(direction, groundRotation);
			direction = dx::XMVectorScale(direction, this->velocity * deltaTime);
			dx::XMStoreFloat3(&dir, direction);
			dx::XMFLOAT3 vel = rbComp->GetLinearVelocity();
			cameraObject->GetTransform().SetPosition(rbComp->GetPosition()); //add camera-offset
			rbComp->SetLinearVelocity({ dir.x * VELOCITY_MULTIPLIER, vel.y, dir.z * VELOCITY_MULTIPLIER });
			dx::XMVECTOR capsule = dx::XMLoadFloat4(&RESET_ROT);
			rbComp->SetRotation(capsule);
			phy.MutexUnlock();

			//if (KEY_PRESSED(Space)) // FPcam //jump is  scuffed
			//{
			//	phy.MutexLock();
			//	if (GetOwner()->HasComponent<RigidBodyComponent>())
			//		GetOwner()->GetComponent<RigidBodyComponent>()->AddForce({ 0.f, 600.f, 0.f });
			//	phy.MutexUnlock();
			//}


		}
	}
	else
	{
		phy.MutexLock();
		//	Input::Instance().FreeMouse();

		rbComp->SetLinearVelocity({ 0.f, 0.f, 0.f });
		dx::XMVECTOR capsule = dx::XMLoadFloat4(&RESET_ROT);
		rbComp->SetRotation(capsule);
		phy.MutexUnlock();
	}

#if NDEBUG 

	// fixes a bug in release where the compiler removes the variables
	// Later: fix these variables?
	speed = 0.0f;
	direction = { 0,0,0 };
#endif // NDEBUG 
}
