#include "ControllerComponent.h"

ControllerComponent::ControllerComponent()
{
	//this->move =1.f;
	this->boost = 4.5f;// *this->move;
	this->crouchSpeed = 0.3f;// *this->move;

	this->lastMousePos = Input::Instance().GetMousePos(); 
	this->showCursor = true;
	this->canRotate = false;
	this->sensetivity = 1.f;
	Input::Instance().SetMouseMode(DirectX::Mouse::MODE_ABSOLUTE);
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
	dx::XMFLOAT2 mouseVec;
	float speed = 1.f;
	mouseVec.x = this->lastMousePos.x - Input::Instance().GetMousePos().x;
	mouseVec.y = this->lastMousePos.y - Input::Instance().GetMousePos().y;
	this->lastMousePos = Input::Instance().GetMousePos();
	//std::cout <<"["<<mouseVec.x<<", "<< mouseVec.y <<"]"<< std::endl;

	if (LMOUSE_DOWN)
	{
		this->canRotate = !this->canRotate;
	}
	if (this->canRotate)
	{
		Input::Instance().ConfineMouse();

		//Can't get mouse cursor to be at position without f-up rotation
		//if (abs(Input::Instance().GetMousePos().x - 400)>100|| abs(Input::Instance().GetMousePos().y - 400) > 100)
		//{
		////	SetCursorPos(400, 400); //set to center
		////	//SetPhysicalCursorPos(400, 400);
		////	this->lastMousePos.x = 0;
		////	this->lastMousePos.y = 0;
		////	mouseVec.x = 0;
		////	mouseVec.y = 0;
		//}
		GetOwner()->GetTransform().Rotate(-mouseVec.y * deltaTime * this->sensetivity, -mouseVec.x * deltaTime * this->sensetivity, 0.f);
		//GetOwner()->GetTransform().Rotate(Input::Instance().GetMousePos().x, Input::Instance().GetMousePos().y,0.f);
	}
	else
		Input::Instance().FreeMouse();

	if (KEY_DOWN(D1))
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
	GetOwner()->GetTransform().Translate(dir.x,dir.y,dir.z);
}
