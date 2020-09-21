#pragma once
#include "Component.h"
#include "Input.h"
#include "Object.h"
//#include <DirectXMath.h>
namespace dx = DirectX;

const float STRAFE = 1.f;
const float FLY = 1.f;
const float WALK = 1.f;
const dx::XMFLOAT3 CROUCH = { 0,2.f,0 };

class ControllerComponent :public Component
{
private:
	dx::XMVECTOR direction;
	float speed;
	dx::XMVECTOR rotation;

	POINT lastMousePos;
	bool showCursor;
	bool lockMouse;


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