#pragma once
#include "Component.h"
#include "Input.h"
#include "Object.h"
//#include <DirectXMath.h>
namespace dx = DirectX;

class ControllerComponent :public Component
{
private:
	float strafe;
	float elevate;  
	float walk;
	float faster;
	float crouchSpeed;
	const dx::XMFLOAT3 CROUCH = { 0.f,2.f,0.f };

	float speed;
	float speedMulitplier;

	POINT lastMousePos;
	bool showCursor;
	bool canRotate;
	float sensetivity;

public:
	ControllerComponent();
	virtual ~ControllerComponent();

	void SetSpeedMulitplier(float speed);
	float GetSpeedMulitplier()const;

	void Update(const float& deltaTime);

	/*
	move = cameraObject->AddComponent<MoveComponent>();
	objects.push_back(cameraObject);
	*/
};