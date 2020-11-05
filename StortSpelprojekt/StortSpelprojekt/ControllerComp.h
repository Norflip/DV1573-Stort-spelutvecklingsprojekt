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

constexpr float WALK_FOV = 50.f;
constexpr float RUN_FOV = 60.f;

constexpr float CROUCH_VELOCITY = 1.f;
constexpr float CROUCH_ACCELERATION = 0.03f;
constexpr float WALK_VELOCITY = 5.f;
constexpr float WALK_ACCELERATION = 0.063f;
constexpr float RUN_VELOCITY = 12.f;
constexpr float RUN_ACCELERATION = 0.10f;
constexpr float VELOCITY_INC_RATE = 0.001f; //how often

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
	enum MoveState
	{
		IDLE,
		WALKING,
		SPRINTING,
		CROUCHING
	};
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

	void calcVelocity(float acceleration) 
	{
		if (this->velocityTimer >= VELOCITY_INC_RATE)
		{
			this->velocity += acceleration;
			this->velocityTimer = 0.f;
		}
	}
public:
	ControllerComp(Object* cameraObject, Object* houseObject);
	virtual ~ControllerComp();

	void Initialize();
	void Update(const float& deltaTime);
};