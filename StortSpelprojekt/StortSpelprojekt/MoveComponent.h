#pragma once
#include "Component.h"
#include <DirectXMath.h>
#include "Object.h"
namespace dx = DirectX;
class MoveComponent:public Component
{
private:
	dx::XMVECTOR direction;
public:
	MoveComponent()
	{
		DirectX::XMFLOAT3 dir = { 0, 1.f, 0 };
		this->direction = dx::XMLoadFloat3(&dir);
	}
	~MoveComponent()
	{
	}

	void Move(DirectX::XMFLOAT3 moveVector)
	{
		dx::XMVECTOR move = dx::XMLoadFloat3(&moveVector);
		dx::XMVECTOR newPos = dx::XMVectorAdd(GetOwner()->GetTransform().GetPosition(),move);
		GetOwner()->GetTransform().SetPosition(newPos);
	}
	void Update(const float& deltaTime)
	{
		if (GetOwner()->GetTransform().GetPosition().m128_f32[1] > 1.f ||
			GetOwner()->GetTransform().GetPosition().m128_f32[1] < -1.f)
		{
			this->direction = dx::XMVectorScale(this->direction,-1);
		}
		dx::XMVECTOR move = dx::XMVectorScale(this->direction, deltaTime);
		dx::XMVECTOR newPos = dx::XMVectorAdd(GetOwner()->GetTransform().GetPosition(), move);
		GetOwner()->GetTransform().SetPosition(newPos);
	}
	/*
	move = cameraObject->AddComponent<MoveComponent>();
	objects.push_back(cameraObject);
	*/


};