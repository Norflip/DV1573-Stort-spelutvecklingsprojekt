#pragma once
#include "Component.h"
#include "NodeWalkerComp.h"
#include "PlayerComp.h"
#include "Input.h"
#include "Object.h"
#include "math.h"
namespace dx = DirectX;
constexpr float CLAMP_X = 90.f * Math::ToRadians;
constexpr float CLAMP_Y = 180.f * Math::ToRadians;


constexpr float WALK_FOV = 50.f;
//constexpr float RUN_FOV = 60.f;


constexpr float CROUCH_VELOCITY = 0.6f;
constexpr float CROUCH_ACCELERATION = 0.03f;
constexpr float WALK_VELOCITY = 4.f;
constexpr float WALK_ACCELERATION = 0.063f;
constexpr float RUN_VELOCITY = 7.f;
constexpr float RUN_ACCELERATION = 0.10f;
constexpr float VELOCITY_INC_RATE = 0.001f; //how often

constexpr float CROUCH_OFFSET_PER = 0.02f;
constexpr float CROUCH_LIMIT = -0.5f;
constexpr float CROUCH_INC_RATE = 0.005f; //how often changes occur
constexpr float JUMP_VELOCITY = 5.f;
constexpr float SIT_RADIUS = 10.f;

constexpr dx::XMFLOAT3 RESET_POS = {20.f,6.f,20.f};
constexpr dx::XMFLOAT4 RESET_ROT = { 0.f,0.f,0.f,1.f };
constexpr dx::XMFLOAT3 DOWN_VEC = { 0.f,-1.f,0.f };

class PlayerComp;
class NodeWalkerComp;
class CameraComponent;
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

	float velocity;
	float velocityTimer; //use timer class??
	float crouchTimer;

	bool freeCam;
	bool showCursor;
	bool canRotate;
	bool isGrounded;
	bool inside;
	bool inDoorRange;
	bool first;

	dx::XMFLOAT3 houseVelocity;
	dx::XMFLOAT3 jumpDir; 
	dx::XMFLOAT3 cameraOffset;
	dx::XMFLOAT3 cameraEuler;
	dx::XMFLOAT3 outsidePos;

	//dx::XMFLOAT3 cameraEuler2;
	Object* cameraObject;
	NodeWalkerComp* houseWalkComp;
	RigidBodyComponent* rbComp;
	CameraComponent* camComp;
	CapsuleColliderComponent* capsuleComp;
	PlayerComp* playerComp;
	World* world;
	Object* house;

	void CheckGrounded();

	MoveState isMoving; 

	void CalcVelocity(float acceleration)
	{
		if (this->velocityTimer >= VELOCITY_INC_RATE)
		{
			this->velocity += acceleration;
			this->velocityTimer = 0.f;
		}
	};
protected:
	//dx::XMVECTOR groundRotation2;
	dx::XMVECTOR groundRotation;
public:
	ControllerComp(Object* cameraObject, Object* houseObject, World* world);
	virtual ~ControllerComp();

	RigidBodyComponent* GetRigidBodyComp() { return this->rbComp; }
	void Initialize();
	void Update(const float& deltaTime);
	float GetVelocity()const { return this->velocity; }
	bool GetInRange()const { return this->inDoorRange; }
	void SetInRange(bool state) { this->inDoorRange = state; }
	bool GetInside()const { return this->inside; }
	void SetInside(bool state) { this->inside = state; }
};