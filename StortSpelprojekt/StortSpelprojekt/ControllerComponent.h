#pragma once
#include "Component.h"
#include "Input.h"
#include "Object.h"
#include "math.h"
namespace dx = DirectX;
constexpr float CLAMP_X = 1.5f;//1.5f;
constexpr float CLAMP_X2 = 0.f;
constexpr float WALK_FOV = 60.f;
constexpr float RUN_FOV = 90.f;
constexpr float FOV_INC = 0.5f; //how much fov increments each time
constexpr float FOV_INC_RATE = 0.01f; //rate of fov incrementation in seconds (so

class ControllerComponent :public Component
{
private:
	//float move;
	float boost;
	float crouchSpeed; 
	const dx::XMFLOAT3 CROUCH = { 0.f,2.f,0.f }; //eventually fix gradient crouch like how fov works
	const dx::XMFLOAT3 DOWN = { 0.f,-1.f,0.f };

	float xClamp;
	float xClamp2;
	bool freeCam;
	bool showCursor;
	bool canRotate;
	float sensetivity;

	float fov;
	float fovTimer;
	CameraComponent* theCamera;

public:
	ControllerComponent();
	virtual ~ControllerComponent();

	void AssignCameraComponent(CameraComponent*);
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