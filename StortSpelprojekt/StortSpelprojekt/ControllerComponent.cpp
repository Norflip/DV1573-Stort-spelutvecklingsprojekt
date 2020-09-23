#include "ControllerComponent.h"

ControllerComponent::ControllerComponent()
{

	this->strafe = 1.f;
	this->elevate = 1.f;
	this->walk = 1.f;
	this->faster = 4.5f * this->walk;
	this->crouchSpeed = 0.3f * this->walk;
	this->speed = 1.f;
	this->speedMulitplier = 1.f;

	this->lastMousePos = Input::Instance().GetMousePos(); 
	this->showCursor = true;
	this->canRotate = true;
	this->sensetivity = 1.f;
}

ControllerComponent::~ControllerComponent()
{
}

void ControllerComponent::SetSpeedMulitplier(float speed)
{
	this->speedMulitplier = speed;
}

float ControllerComponent::GetSpeedMulitplier() const
{
	return this->speedMulitplier;
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
		GetOwner()->GetTransform().Rotate(-mouseVec.y*deltaTime* this->sensetivity,-mouseVec.x*deltaTime* this->sensetivity,0.f);
	}

	if (KEY_DOWN(D1))
	{
		this->showCursor = !this->showCursor;
		ShowCursor(this->showCursor);
	}
	
	if (KEY_DOWN(LeftShift))
		this->speed = faster;
	if (KEY_UP(LeftShift))
		this->speed = walk;
	if (KEY_DOWN(LeftControl))
	{
		this->speed = crouchSpeed;
		GetOwner()->GetTransform().Translate(-CROUCH.x, -CROUCH.y, -CROUCH.z);
	}
	if (KEY_UP(LeftControl))
	{
		this->speed = walk;
		GetOwner()->GetTransform().Translate(CROUCH.x, CROUCH.y, CROUCH.z);
	}

	if (KEY_PRESSED(Space))
		dir.y += elevate;
	if (KEY_PRESSED(C))
		dir.y -= elevate;
	if (KEY_PRESSED(W))
		dir.z += walk;
	if (KEY_PRESSED(S))
		dir.z -= walk;
	if (KEY_PRESSED(A))
		dir.x -= strafe;
	if (KEY_PRESSED(D))
		dir.x += strafe;

	dx::XMVECTOR direction = dx::XMLoadFloat3(&dir);
	direction = dx::XMVectorScale(direction, this->speed * this->speedMulitplier);
	direction = dx::XMVectorScale(direction, deltaTime);
	dx::XMStoreFloat3(&dir, direction);
	GetOwner()->GetTransform().Translate(dir.x,dir.y,dir.z);
}
