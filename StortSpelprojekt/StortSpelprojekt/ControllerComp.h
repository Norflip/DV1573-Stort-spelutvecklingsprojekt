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

//constexpr float CROUCH_VELOCITY = 0.5f;
//constexpr float CROUCH_ACCELERATION = 1.f;
constexpr float WALK_VELOCITY = 8.f;
constexpr float WALK_ACCELERATION = 1.5f;
constexpr float RUN_VELOCITY = 20.f;
constexpr float RUN_ACCELERATION = 3.5f;
constexpr float VELOCITY_INC_RATE = 0.1f;
constexpr float VELOCITY_MULTIPLIER = 20.f;

constexpr dx::XMFLOAT3 RESET_POS = {20.f,3.f,20.f};
constexpr dx::XMFLOAT4 RESET_ROT = { 0.f,0.f,0.f,1.f };

class ControllerComp :public Component //PlayerControllerComp
{
private:

	//fixa jump isGrounded/collisioninfo 
	//addForce fix??

	float velocity;
	float velocityTimer; //use timer class??
	
	//const dx::XMFLOAT3 CROUCH = { 0.f,2.f,0.f }; //eventually fix gradient crouch like how fov works
	//const dx::XMFLOAT3 DOWN = { 0.f,-1.f,0.f };

	float xClamp;
	bool freeCam;
	bool showCursor;
	bool canRotate;
	dx::XMFLOAT4 groundQuaterion;

	float fov;
	float fovTimer; //use timer class??
	Object* cameraObject;
	RigidBodyComponent* rbComp;
	CameraComponent* camComp;
	CapsuleColliderComponent* capsuleComp;

public:
	ControllerComp();
	ControllerComp(Object* cameraObject);
	virtual ~ControllerComp();

	void Initialize();
	void Update(const float& deltaTime);
};