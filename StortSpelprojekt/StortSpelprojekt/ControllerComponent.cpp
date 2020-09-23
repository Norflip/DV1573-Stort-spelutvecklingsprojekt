#include "ControllerComponent.h"

ControllerComponent::ControllerComponent()
{
	DirectX::XMFLOAT3 dir = { 0, 1.f, 0 };
	this->direction = dx::XMLoadFloat3(&dir);
	this->speed = 1.f;
}

ControllerComponent::~ControllerComponent()
{
}

void ControllerComponent::Update(const float& deltaTime)
{

	DirectX::XMFLOAT3 dir = { 0,0,0 };
	if (KEY_PRESSED(LeftShift))
		this->speed = 4.5f;
	else
		this->speed = walk;
	if (KEY_PRESSED(Space))
		dir.y += fly;
	if (KEY_PRESSED(C)|| KEY_PRESSED(LeftControl))
		dir.y -= fly;
	if (KEY_PRESSED(W))
		dir.z += walk;
	if (KEY_PRESSED(S))
		dir.z -= walk;
	if (KEY_PRESSED(A))
		dir.x -= strafe;
	if (KEY_PRESSED(D))
		dir.x += strafe;

	this->direction = dx::XMLoadFloat3(&dir);
	this->direction = dx::XMVectorScale(this->direction, this->speed);
	this->direction = dx::XMVectorScale(this->direction, deltaTime);
	dx::XMVECTOR newPos = dx::XMVectorAdd(GetOwner()->GetTransform().GetPosition(), this->direction);
	GetOwner()->GetTransform().SetPosition(newPos);
}
