#pragma once
#include "Component.h"
#include "NodeWalkerComp.h"
#include "Input.h"
#include "Object.h"
#include "math.h"
#include <algorithm>
namespace dx = DirectX;
constexpr float CLAMP_X = 90.f * Math::ToRadians;
constexpr float CLAMP_Y = 360.f * Math::ToRadians;

constexpr float WALK_FOV = 70.f;
constexpr float RUN_FOV = 90.f;
//constexpr float FOV_INC = 0.5f; //how much fov increments each time
//constexpr float FOV_INC_RATE = 0.01f; //rate of fov incrementation in seconds (so

constexpr float CROUCH_VELOCITY = 0.5f;
constexpr float CROUCH_ACCELERATION = 1.f;
constexpr float WALK_VELOCITY = 4.f;
constexpr float WALK_ACCELERATION = 5.0f;
constexpr float RUN_VELOCITY = 10.f;
constexpr float RUN_ACCELERATION = 5.5f;
constexpr float VELOCITY_INC_RATE = 0.1f; //how often

constexpr float CROUCH_OFFSET_PER = 0.2f;
constexpr float CROUCH_LIMIT = -0.5f;
constexpr float CROUCH_INC_RATE = 0.01f; //how often changes occur
constexpr float JUMP_VELOCITY = 5.f;

constexpr dx::XMFLOAT3 RESET_POS = {20.f,3.f,20.f};
constexpr dx::XMFLOAT4 RESET_ROT = { 0.f,0.f,0.f,1.f };
constexpr dx::XMFLOAT3 DOWN_VEC = { 0.f,-1.f,0.f };

class ControllerComp :public Component 
{
private:

	//fixa fov based on speed
	//fox wouldn't change when standing still
	//addForce fix??
	float fov;
	float fovTimer; //use timer class??
	float velocity;
	float velocityTimer; //use timer class??
	
	float crouchTimer;

	bool freeCam;
	bool showCursor;
	bool canRotate;
	dx::XMFLOAT3 jumpDir; 
	dx::XMFLOAT3 cameraOffset;
	dx::XMFLOAT3 cameraEuler;

	Object* cameraObject;
	Object* houseObject;
	RigidBodyComponent* rbComp;
	CameraComponent* camComp;
	CapsuleColliderComponent* capsuleComp;

	bool IsGrounded() const;
public:
	//ControllerComp();
	ControllerComp(Object* cameraObject, Object* houseObject);
	virtual ~ControllerComp();

	void Initialize();
	void Update(const float& deltaTime);
};