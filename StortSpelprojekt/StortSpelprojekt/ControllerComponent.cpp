#include "ControllerComponent.h"

ControllerComponent::ControllerComponent()
{
	//this->move =1.f;
	this->boost = 8.f;// *this->move;
	this->crouchSpeed = 0.3f;// *this->move;

	this->freeCam = true;
	this->showCursor = true;
	this->canRotate = false;
	this->sensetivity = 1.f;
	this->fov = 60.f;
	this->fovTimer = 0.f;
	this->theCamera = nullptr;
}

ControllerComponent::~ControllerComponent()
{
}

void ControllerComponent::AssignCameraComponent(CameraComponent* component)
{
	this->theCamera = component;
}

//void ControllerComponent::SetMoveSpeed(float speed) //sets move speed
//{
//	this->move = speed;
//}
//
//float ControllerComponent::GetMoveSpeed() const
//{
//	return this->move;
//}

void ControllerComponent::SetBoostSpeed(float speed) //sets boost multiplier (boost*move)
{
	this->boost = speed;
}

float ControllerComponent::GetBoostSpeed() const
{
	return this->boost;
}

void ControllerComponent::SetCrouchSpeed(float speed) //sets crouch multiplier (crouch*move)
{
	this->crouchSpeed = speed;
}

float ControllerComponent::GetCrouchSpeed() const
{
	return crouchSpeed;
}

void ControllerComponent::SetSensetivity(float sensetivity)
{
	this->sensetivity = sensetivity;
}

float ControllerComponent::GetSensetivity() const
{
	return this->sensetivity;
}

void ControllerComponent::Update(const float& deltaTime)
{
	DirectX::XMFLOAT3 dir = { 0.f,0.f,0.f };
	float speed = 1.f;
	this->fovTimer += deltaTime;
	dx::XMVECTOR groundRotation;
	//float fov = 60.f;
	//dx::XMFLOAT3 origin;
	//dx::XMStoreFloat3(&origin, GetOwner()->GetTransform().GetPosition());
	//Ray ray(origin,DOWN);
	//DShape::DrawLine(ray.origin, ray.GetPoint(1000.0f), { 1,1,0 });

	//Physics& phy = Physics::Instance();
	//RayHit hit;

	//if (phy.RaytestSingle(ray, 1000.0f, hit, FilterGroups::DEFAULT))
	//{
	//	DShape::DrawLine(ray.origin, hit.position, { 1,1,0 });
	//	DShape::DrawSphere(hit.position, 1.0f, { 0, 0, 1 });

	//	if (hit.object != nullptr)
	//	{
	//		std::cout <<"yo "<< hit.object->GetName() << std::endl;
	//	}
	//}

	if (KEY_DOWN(V))
		this->freeCam = !this->freeCam;


	if (KEY_DOWN(F))
	{
		this->canRotate = !this->canRotate;
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

		Transform& transform = GetOwner()->GetTransform();
		dx::XMVECTOR right = transform.TransformDirection({ 1,0,0 });
		dx::XMVECTOR eulerRotation = dx::XMQuaternionMultiply(dx::XMQuaternionRotationAxis(right, x), dx::XMQuaternionRotationAxis({ 0,1,0 }, y));

		//float x2 = Input::Instance().GetMousePosRelative().x * deltaTime;
		//xClamp2 += x2;
		//if (xClamp2 > CLAMP_X2)
		//{
		//	xClamp2 = CLAMP_X2;
		//	x2 = 0.f;
		//}
		//else if (xClamp2 < -CLAMP_X2)
		//{
		//	xClamp2 = -CLAMP_X2;
		//	x2 = 0.f;
		//}

		//groundRotation = dx::XMQuaternionMultiply(dx::XMQuaternionRotationAxis(right, x2), dx::XMQuaternionRotationAxis({ 0,1,0 }, y));
		//groundRotation = dx::XMQuaternionMultiply(transform.GetRotation(), groundRotation);


		transform.SetRotation(dx::XMQuaternionMultiply(transform.GetRotation(), eulerRotation));
		Input::Instance().ResetRelative();




		if (KEY_DOWN(O))
		{
			this->showCursor = !this->showCursor;
			ShowCursor(this->showCursor);
		}
		//std::cout << "Timer : " << fovTimer <<", fov: "<<this->fov<< std::endl;
		if (KEY_PRESSED(LeftShift)) //sprint
		{
			speed = boost;
			if (this->fov < RUN_FOV && this->fovTimer >= FOV_INC_RATE)
			{
				this->fov += FOV_INC;
				this->fovTimer = 0.f;
			}
		}
		else
		{
			if (this->fov > WALK_FOV && this->fovTimer >= FOV_INC_RATE)
			{
				this->fov -= FOV_INC;
				this->fovTimer = 0.f;
			}
		}
		if (theCamera)
			theCamera->SetFOV(fov);


		if (KEY_PRESSED(W))
			dir.z += 1.f;// move;
		if (KEY_PRESSED(S))
			dir.z -= 1.f;// move;
		if (KEY_PRESSED(A))
			dir.x -= 1.f;// move;
		if (KEY_PRESSED(D))
			dir.x += 1.f;// move;


		if (freeCam)
		{
			if (KEY_PRESSED(Space)) //Free cam
				dir.y += 1.f;// move;
			if (KEY_PRESSED(C))
				dir.y -= 1.f;// move;

	
			//direction = GetOwner()->GetTransform().TransformDirection(direction);
		}
		else
		{
			if (KEY_DOWN(LeftControl))
			{
				speed = crouchSpeed;
				GetOwner()->GetTransform().Translate(-CROUCH.x, -CROUCH.y, -CROUCH.z);
			}
			if (KEY_UP(LeftControl))
			{
				speed = 1.f;// move;
				GetOwner()->GetTransform().Translate(CROUCH.x, CROUCH.y, CROUCH.z);
			}

			//dx::XMFLOAT4 temp;
			//dx::XMStoreFloat4(&temp, groundRotation);
			//std::cout << "x: " << temp.x << ", y: " << temp.y << ", z: " << temp.z << ", w: " << temp.w << std::endl;
			//direction = GetOwner()->GetTransform().TransformDirectionCustomRotation(direction, groundRotation);
		}

		dx::XMVECTOR direction = dx::XMLoadFloat3(&dir);
		direction = dx::XMVector3Normalize(direction);

		//dx::XMFLOAT4 temp;
		//dx::XMStoreFloat4(&temp, groundRotation);
		//std::cout << "x: " << temp.x << ", y: " << temp.y << ", z: " << temp.z << ", w: " << temp.w << std::endl;
		//direction = GetOwner()->GetTransform().TransformDirectionCustomRotation(direction, groundRotation);
		direction = GetOwner()->GetTransform().TransformDirection(direction);
		direction = dx::XMVectorScale(direction, speed * deltaTime);
		dx::XMStoreFloat3(&dir, direction);
		GetOwner()->GetTransform().Translate(dir.x, dir.y, dir.z);

	}
	//else
	//	Input::Instance().FreeMouse();


#if NDEBUG 

	// fixes a bug in release where the compiler removes the variables
	speed = 0.0f;
	direction = { 0,0,0 };
#endif // NDEBUG 
}
