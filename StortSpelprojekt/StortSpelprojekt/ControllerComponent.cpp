#include "ControllerComponent.h"

ControllerComponent::ControllerComponent()
{
	//this->move =1.f;
	this->boost = 4.5f;// *this->move;
	this->crouchSpeed = 0.3f;// *this->move;

	this->showCursor = true;
	this->canRotate = false;
	this->sensetivity = 1.f;
}

ControllerComponent::~ControllerComponent()
{
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
	if (KEY_DOWN(F))
	{
		this->canRotate = !this->canRotate;
		if (this->canRotate)
		{
			Input::Instance().SetMouseMode(dx::Mouse::MODE_RELATIVE);
		}
		else
		{
			Input::Instance().SetMouseMode(dx::Mouse::MODE_ABSOLUTE);
		}
	}
	if (this->canRotate)
	{
		//Input::Instance().ConfineMouse();
		SetCursorPos(400, 400); //set this to coordinates middle of screen? get height/width from input?
		GetOwner()->GetTransform().Rotate(Input::Instance().GetMousePosRelative().x * deltaTime, Input::Instance().GetMousePosRelative().y * deltaTime, 0.0f);
		Input::Instance().ResetRelative();
	}
	//else
	//	Input::Instance().FreeMouse();

	if (KEY_DOWN(F))
	{
		this->showCursor = !this->showCursor;
		ShowCursor(this->showCursor);
	}
	
	if (KEY_PRESSED(LeftShift))
		speed = boost;
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

	if (KEY_PRESSED(Space))
		dir.y += 1.f;// move;
	if (KEY_PRESSED(C))
		dir.y -= 1.f;// move;
	if (KEY_PRESSED(W))
		dir.z += 1.f;// move;
	if (KEY_PRESSED(S))
		dir.z -= 1.f;// move;
	if (KEY_PRESSED(A))
		dir.x -= 1.f;// move;
	if (KEY_PRESSED(D))
		dir.x += 1.f;// move;

	dx::XMVECTOR direction = dx::XMLoadFloat3(&dir);

	//direction = dx::XMVector3Normalize(direction);
	direction = GetOwner()->GetTransform().TransformDirection(direction);
	direction = dx::XMVectorScale(direction, speed);
	direction = dx::XMVectorScale(direction, deltaTime);
	dx::XMStoreFloat3(&dir, direction);
	GetOwner()->GetTransform().Translate(dir.x, dir.y, dir.z);

#if NDEBUG 

	// fixes a bug in release where the compiler removes the variables
	speed = 0.0f;
	direction = { 0,0,0 };
#endif // NDEBUG 
}
