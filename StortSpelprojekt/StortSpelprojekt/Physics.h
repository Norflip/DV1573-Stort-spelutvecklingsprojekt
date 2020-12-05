#pragma once
#include "CollisionInfo.h"
#include "Ray.h"
#include "PhysicCallbacks.h"

enum class FilterGroups : unsigned short
{
	//EVERYTHING = -1,
	NOTHING = 0,
	DEFAULT = 1 << 0,
	TERRAIN = 1 << 1,
	PICKUPS = 1 << 2,
	PLAYER = 1 << 3,
	ENEMIES = 1 << 4,
	PROPS = 1 << 5,
	HOLDABLE = 1 << 6,
	DOOR = 1 << 7,
	FIRE = 1 << 8,
	CLICKABLE = 1 << 9,
	PUZZLE = 1 << 10,
	EVERYTHING = DEFAULT | TERRAIN | PICKUPS | PLAYER | ENEMIES | PROPS | HOLDABLE | DOOR | FIRE | CLICKABLE | PUZZLE
};
DEFINE_ENUM_FLAG_OPERATORS(FilterGroups)

#define LOG_REACT TRUE

class RigidBodyComponent;
constexpr dx::XMFLOAT3 DEFAULT_GRAVITY = { 0,-10.0f,0 };

class Physics
{
public:
	Physics();
	virtual ~Physics();

	void Initialize(dx::XMFLOAT3 gravity = DEFAULT_GRAVITY);
	
	// remove rigidbody on chunk and add shape?
	//void ReigsterCollisionObject();
	rp::PhysicsWorld* GetWorld() const { return this->world; }
	rp::PhysicsCommon& GetCommon() { return this->common; }
	void MutexLock();
	void MutexUnlock();

	void FixedUpdate(const float& fixedDeltaTime);

	bool RaytestSingle(const Ray& ray, float maxDistance, RayHit& hit, FilterGroups group = FilterGroups::EVERYTHING) const;

private:

	CollisionEventListener listener;
	rp::Vector3 gravity;
	rp::PhysicsWorld* world;
	rp::PhysicsCommon common;
	
	std::mutex physicsThreadMutex;
	std::unordered_map<size_t, RigidBodyComponent*> bodyMap;
};