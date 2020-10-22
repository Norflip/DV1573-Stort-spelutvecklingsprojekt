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
constexpr float VELOCITY_INC_RATE = 0.1f; //how often
constexpr float VELOCITY_MULTIPLIER = 20.f;

constexpr float CROUCH_OFFSET_PER = 0.2f;
constexpr float CROUCH_LIMIT = -2.f;
constexpr float CROUCH_INC_RATE = 0.01f; //how often
constexpr float JUMP_VELOCITY = 5.f;

constexpr dx::XMFLOAT3 RESET_POS = {20.f,3.f,20.f};
constexpr dx::XMFLOAT4 RESET_ROT = { 0.f,0.f,0.f,1.f };
constexpr dx::XMFLOAT3 DOWN = { 0.f,-1.f,0.f };


class ControllerComp :public Component //PlayerControllerComp
{
private:

	//fixa jump isGrounded/collisioninfo 
	//addForce fix??
	float fov;
	float fovTimer; //use timer class??
	float velocity;
	float velocityTimer; //use timer class??
	
	float crouchTimer;
	//const dx::XMFLOAT3 CROUCH = { 0.f,2.f,0.f }; //eventually fix gradient crouch like how fov works
	//
	dx::XMFLOAT3 cameraOffset;
	

	float xClamp;
	bool freeCam;
	bool showCursor;
	bool canRotate;
	dx::XMFLOAT4 groundQuaterion;

	Object* cameraObject;
	RigidBodyComponent* rbComp;
	CameraComponent* camComp;
	CapsuleColliderComponent* capsuleComp;

	bool IsGrounded() const;
public:
	//ControllerComp();
	ControllerComp(Object* cameraObject);
	virtual ~ControllerComp();

	void Initialize();
	void Update(const float& deltaTime);
};