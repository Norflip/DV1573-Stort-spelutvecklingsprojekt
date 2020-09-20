#pragma once
#include "Component.h"
#include "Input.h"
#include "Object.h"
//#include <DirectXMath.h>
namespace dx = DirectX;

const float strafe = 1.f;
const float fly = 1.f;
const float walk = 1.f;

class ControllerComponent :public Component
{
private:
	dx::XMVECTOR direction;
	float speed;
public:
	ControllerComponent();
	virtual ~ControllerComponent();

	//void Move(DirectX::XMFLOAT3 moveVector);
	void Update(const float& deltaTime);

	/*
	move = cameraObject->AddComponent<MoveComponent>();
	objects.push_back(cameraObject);
	*/
};