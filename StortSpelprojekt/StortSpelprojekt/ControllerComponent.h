#pragma once
#include "Component.h"
#include "Input.h"
#include "Object.h"
#include "math.h"
namespace dx = DirectX;
constexpr float CLAMP_X = 1.5f;
constexpr float WALK_FOV = 60.f;
constexpr float RUN_FOV = 90.f;
constexpr float FOV_INC = 0.5f; //how much fov increments each time
constexpr float FOV_INC_RATE = 0.01f; //rate of fov incrementation in seconds (so


constexpr float CROUCH_VELOCITY = 0.5f;
constexpr float CROUCH_ACCELERATION = 1.f;
constexpr float WALK_VELOCITY = 8.f;
constexpr float WALK_ACCELERATION = 1.5f;
constexpr float RUN_VELOCITY = 20.f;
constexpr float RUN_ACCELERATION = 3.5f;
constexpr float VELOCITY_INC_RATE = 0.1f;

constexpr dx::XMFLOAT3 RESET_POS = {20.f,3.f,20.f};

class ControllerComponent :public Component //PlayerControllerComp
{
private:

	//float acceleration;
	//float deacceleration;

	//fixa jump isGrounded/collisioninfo 
	//addForce fix


	//dx::xmfloat3 force
	//F=C*A*(p*V^2)/2
	//dx::xmfloat3 drag

	float velocity;
	float velocityTimer;
	//acceleration //when walking (slowly) increase speed etc
	//deacceleraion
	//jump
	//float boost;
	//float crouchSpeed; 
	const dx::XMFLOAT3 CROUCH = { 0.f,2.f,0.f }; //eventually fix gradient crouch like how fov works
	const dx::XMFLOAT3 DOWN = { 0.f,-1.f,0.f };

	float xClamp;
	bool freeCam;
	bool showCursor;
	bool canRotate;
	dx::XMFLOAT4 groundQuaterion;

	float fov;
	float fovTimer; //use timer

public:
	ControllerComponent();
	virtual ~ControllerComponent();

	//void SetBoostSpeed(float);
	//float GetBoostSpeed()const;

	//void SetCrouchSpeed(float);
	//float GetCrouchSpeed()const;

	//void SetSensetivity(float);
	//float GetSensetivity()const;
	void Initialize();
	void Update(const float& deltaTime);

	/*
	move = cameraObject->AddComponent<MoveComponent>();
	objects.push_back(cameraObject);
	*/
};