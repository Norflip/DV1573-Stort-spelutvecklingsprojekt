#include "ControllerComponent.h"

ControllerComponent::ControllerComponent()
{
	//this->move =1.f;
	//this->boost = 20.f;// *this->move;
	//this->crouchSpeed = 0.3f;// *this->move;
	this->velocity = 0.f;
	this->velocityTimer = 0.f;

	this->freeCam = true;
	this->showCursor = true;
	this->canRotate = false;
	//this->sensetivity = 1.f;
	this->fov = 60.f;
	this->fovTimer = 0.f;
	//this->theCamera = nullptr;
	this->groundQuaterion = { 0.f,0.f,0.f,1.f };
}

ControllerComponent::~ControllerComponent()
{
}

void ControllerComponent::Initialize()
{
	if (GetOwner()->HasComponent<RigidBodyComponent>()|| GetOwner()->HasComponent<CapsuleColliderComponent>()
		|| GetOwner()->HasComponent<CameraComponent>())
	{
		GetOwner()->GetComponent<RigidBodyComponent>()->LockRotation(true);
		GetOwner()->GetComponent<RigidBodyComponent>()->SetLinearDamping(0.1f);
		GetOwner()->GetComponent<RigidBodyComponent>()->GetRigidBody()->getCollider(0)->getMaterial().setBounciness(0.f);
		GetOwner()->GetComponent<RigidBodyComponent>()->EnableGravity(!this->freeCam);
		GetOwner()->GetComponent<RigidBodyComponent>()->SetLinearVelocity({ 0.f, 0.f, 0.f });
	}
	else 
	{
		std::cout << "component missing" << std::endl;
	}
}

void ControllerComponent::Update(const float& deltaTime)
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
		GetOwner()->GetComponent<RigidBodyComponent>()->SetPosition(resetPos);
		GetOwner()->GetComponent<RigidBodyComponent>()->SetLinearVelocity({ 0.f, 0.f, 0.f });
	}
	if (KEY_DOWN(O))
	{
		this->showCursor = !this->showCursor;
		ShowCursor(this->showCursor);
	}
	if (KEY_DOWN(V))
	{
		this->freeCam = !this->freeCam;
		GetOwner()->GetComponent<RigidBodyComponent>()->SetLinearVelocity({ 0.f, 0.f, 0.f });
		GetOwner()->GetComponent<RigidBodyComponent>()->EnableGravity(!this->freeCam);
	}
	if (KEY_DOWN(F))
	{
		this->canRotate = !this->canRotate;
		GetOwner()->GetComponent<RigidBodyComponent>()->SetLinearVelocity({ 0.f, 0.f, 0.f });
		if (this->canRotate)
			Input::Instance().SetMouseMode(dx::Mouse::MODE_RELATIVE);
		else
			Input::Instance().SetMouseMode(dx::Mouse::MODE_ABSOLUTE);
	}

	if (this->canRotate)
	{
		//Input::Instance().ConfineMouse();
		//SetCursorPos(400, 400); //set this to coordinates middle of screen? get height/width from input?

		float x = Input::Instance().GetMousePosRelative().x * deltaTime;
		float y = Input::Instance().GetMousePosRelative().y * deltaTime;
		xClamp += x;
		if (xClamp > CLAMP_X)
		{
			xClamp = CLAMP_X;
			x = 0.f;
		}
		if (xClamp < -CLAMP_X)
		{
			xClamp = -CLAMP_X;
			x = 0.f;
		}

		//rotate view
		Transform& transform = GetOwner()->GetTransform();
		dx::XMVECTOR right = transform.TransformDirection({ 1,0,0 });
		dx::XMVECTOR eulerRotation = dx::XMQuaternionMultiply(dx::XMQuaternionRotationAxis(right, x), dx::XMQuaternionRotationAxis({ 0,1,0 }, y));

		//rotate walking direction to allign to ground
		groundRotation = dx::XMQuaternionMultiply(dx::XMQuaternionRotationAxis(right, 0), dx::XMQuaternionRotationAxis({ 0,1,0 }, y));
		groundRotation = dx::XMQuaternionMultiply(dx::XMLoadFloat4(&this->groundQuaterion), groundRotation);
		dx::XMStoreFloat4(&groundQuaterion, groundRotation); 

		//testing rotation of capsule/rigidbody
		//dx::XMVECTOR rbQuaternion = GetOwner()->GetComponent<RigidBodyComponent>()->GetRotation();
		//dx::XMVECTOR rbRot = dx::XMQuaternionMultiply(dx::XMQuaternionRotationAxis(right, 0), dx::XMQuaternionRotationAxis({ 0,1,0 }, y));
		//rbRot = dx::XMQuaternionMultiply(rbRot, rbQuaternion); 

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
		if (GetOwner()->HasComponent<CameraComponent>())
			GetOwner()->GetComponent<CameraComponent>()->SetFOV(fov);

		if (KEY_PRESSED(W) || KEY_PRESSED(S) || KEY_PRESSED(A) || KEY_PRESSED(D))
		{
			if (KEY_PRESSED(W))
				dir.z += 1.f;// move;
			if (KEY_PRESSED(S))
				dir.z -= 1.f;// move;
			if (KEY_PRESSED(A))
				dir.x -= 1.f;// move;
			if (KEY_PRESSED(D))
				dir.x += 1.f;// move;

			if (this->velocity < WALK_VELOCITY && velocityTimer >= VELOCITY_INC_RATE)
			{
				this->velocity += WALK_ACCELERATION;
				this->velocityTimer = 0.f;
			}
		}
		else
			if (this->velocity > 0.f && velocityTimer >= VELOCITY_INC_RATE)
			{
				this->velocity -= WALK_ACCELERATION;
				this->velocityTimer = 0.f;
			}

		if (freeCam) //flying camera
		{
			if (KEY_PRESSED(Space)) //Free cam
				dir.y += 1.f;// move;
			if (KEY_PRESSED(C))
				dir.y -= 1.f;// move;

		}
		else //first person specific actions
		{
			//if (KEY_PRESSED(Space)) // FPcam //jump is  scuffed
			//{
			//	phy.MutexLock();
			//	if (GetOwner()->HasComponent<RigidBodyComponent>())
			//		GetOwner()->GetComponent<RigidBodyComponent>()->AddForce({ 0.f, 200.f, 0.f });
			//	phy.MutexUnlock();
			//}

			//if (KEY_DOWN(LeftControl))
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
			direction = GetOwner()->GetTransform().TransformDirection(direction);
			direction = dx::XMVectorScale(direction, this->velocity * deltaTime);
			dx::XMStoreFloat3(&dir, direction);
			dx::XMFLOAT3 vel = GetOwner()->GetComponent<RigidBodyComponent>()->GetLinearVelocity();
			GetOwner()->GetComponent<RigidBodyComponent>()->SetLinearVelocity({ dir.x * 50, dir.y * 50, dir.z * 50 });
		}
		else
		{
			direction = GetOwner()->GetTransform().TransformDirectionCustomRotation(direction, groundRotation);
			direction = dx::XMVectorScale(direction, this->velocity * deltaTime);
			dx::XMStoreFloat3(&dir, direction);
			dx::XMFLOAT3 vel = GetOwner()->GetComponent<RigidBodyComponent>()->GetLinearVelocity();
			GetOwner()->GetComponent<RigidBodyComponent>()->SetLinearVelocity({ dir.x * this->velocity, vel.y, dir.z * this->velocity });
		}
		//direction = dx::XMVectorScale(direction, this->velocity * deltaTime);
		//dx::XMStoreFloat3(&dir, direction);
		//GetOwner()->GetTransform().Translate(dir.x, dir.y, dir.z);

		phy.MutexLock();
		if (GetOwner()->HasComponent<RigidBodyComponent>())
		{
			//std::cout << "x: " << dir.x << ", y: " << dir.y << ", z: " << dir.z << std::endl;
			//GetOwner()->GetComponent<RigidBodyComponent>()->SetPosition(GetOwner()->GetTransform().GetPosition());
			GetOwner()->GetComponent<RigidBodyComponent>()->SetRotation(GetOwner()->GetTransform().GetRotation());
			//GetOwner()->GetComponent<CapsuleColliderComponent>()->SetRotation(); //
		}
		phy.MutexUnlock();

	}
	//else
	//	Input::Instance().FreeMouse();



#if NDEBUG 

	// fixes a bug in release where the compiler removes the variables
	speed = 0.0f;
	direction = { 0,0,0 };
#endif // NDEBUG 
}
