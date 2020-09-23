#include "ControllerComponent.h"

ControllerComponent::ControllerComponent()
{
	DirectX::XMFLOAT3 dir = { 0.f, 1.f, 0.f };
	this->direction = dx::XMLoadFloat3(&dir);
	this->speed = 1.f;

	this->lastMousePos = Input::Instance().GetMousePos(); 
	this->showCursor = true;
	//this->lockMouse = false;
	this->canRotate = true;
}

ControllerComponent::~ControllerComponent()
{
}

void ControllerComponent::Update(const float& deltaTime)
{
	DirectX::XMFLOAT3 dir = { 0,0,0 };
	dx::XMFLOAT2 mouseVec;
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
		float sens = 0.8f;
		GetOwner()->GetTransform().Rotate(-mouseVec.y*deltaTime*sens,mouseVec.x*deltaTime*sens,0.f);
	}

	if (KEY_DOWN(D1))
	{
		this->showCursor = !this->showCursor;
		ShowCursor(this->showCursor);
		
	}
	//ShowCursor(false); 
	//if (KEY_DOWN(D2))
	//	this->lockMouse = !this->lockMouse;
	//if (this->lockMouse)
	//{
	//	float absX = abs(Input::Instance().GetMousePos().x);
	//	float absY = abs(Input::Instance().GetMousePos().y);
	//	float mouseLockLimit = 80;
	//	if (absX > mouseLockLimit || absY > mouseLockLimit)
	//	{
	//		SetCursorPos(400, 400);
	//	}
	//}
	
	if (KEY_DOWN(LeftShift))
		this->speed = 4.5f;
	if (KEY_UP(LeftShift))
		this->speed = WALK;
	if (KEY_DOWN(LeftControl))
	{
		this->speed = 0.3f;
		dx::XMVECTOR crouchPos = dx::XMLoadFloat3(&CROUCH);
		crouchPos = dx::XMVectorSubtract(GetOwner()->GetTransform().GetPosition(), crouchPos);
		GetOwner()->GetTransform().SetPosition(crouchPos);
	}
	if (KEY_UP(LeftControl))
	{
		this->speed = WALK;
		dx::XMVECTOR crouchPos = dx::XMLoadFloat3(&CROUCH);
		crouchPos = dx::XMVectorAdd(GetOwner()->GetTransform().GetPosition(), crouchPos);
		GetOwner()->GetTransform().SetPosition(crouchPos);
	}

	if (KEY_PRESSED(Space))
		dir.y += FLY;
	if (KEY_PRESSED(C))
		dir.y -= FLY;
	if (KEY_PRESSED(W))
		dir.z += WALK;
	if (KEY_PRESSED(S))
		dir.z -= WALK;
	if (KEY_PRESSED(A))
		dir.x -= STRAFE;
	if (KEY_PRESSED(D))
		dir.x += STRAFE;
	//std::cout << speed << std::endl;
	this->direction = dx::XMLoadFloat3(&dir);
	this->direction = dx::XMVectorScale(this->direction, this->speed);
	this->direction = dx::XMVectorScale(this->direction, deltaTime);

	dx::XMVECTOR newPos = dx::XMVectorAdd(GetOwner()->GetTransform().GetPosition(), this->direction);
	GetOwner()->GetTransform().SetPosition(newPos);
}
