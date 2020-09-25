#pragma once
#include "Component.h"
#include "Input.h"
#include "Object.h"
//#include <DirectXMath.h>
namespace dx = DirectX;

class ControllerComponent :public Component
{
private:
	//float move;
	float boost;
	float crouchSpeed;
	const dx::XMFLOAT3 CROUCH = { 0.f,2.f,0.f };

	POINT lastMousePos;
	bool showCursor;
	bool canRotate;
	float sensetivity;
public:
	ControllerComponent();
	virtual ~ControllerComponent();

	//void SetMoveSpeed(float);
	//float GetMoveSpeed()const;

	void SetBoostSpeed(float);
	float GetBoostSpeed()const;

	void SetCrouchSpeed(float);
	float GetCrouchSpeed()const;

	void SetSensetivity(float);
	float GetSensetivity()const;

	void Update(const float& deltaTime);

	/*
	move = cameraObject->AddComponent<MoveComponent>();
	objects.push_back(cameraObject);
	*/
};