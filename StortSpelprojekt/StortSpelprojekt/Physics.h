#pragma once
#include <react3d.h>
namespace rp = reactphysics3d;

#include "CollisionInfo.h"
#include "Ray.h"
#include "PhysicCallbacks.h"

#include <vector>
#include <iostream>
#include <unordered_map>
#include <mutex>

enum class FilterGroups : unsigned short
{
	//EVERYTHING = -1,
	NOTHING = 0,
	DEFAULT = 1 << 0,
	TERRAIN = 1 << 1,
	PICKUPS = 1 << 2,
	PLAYER = 1 << 3,
	ENEMIES = 1 << 4,
	TEST_0 = 1 << 5,
	EVERYTHING = DEFAULT | TERRAIN | PICKUPS | PLAYER | ENEMIES | TEST_0
};


DEFINE_ENUM_FLAG_OPERATORS(FilterGroups)

class RigidBodyComponent;

class Physics
{
public:
	Physics();
	virtual ~Physics();

	void Initialize(dx::XMFLOAT3 gravity);
	
	// remove rigidbody on chunk and add shape?
	//void ReigsterCollisionObject();
	rp::PhysicsWorld* GetWorld() const { return this->world; }
	rp::PhysicsCommon& GetCommon() { return this->common; }
	void MutexLock();
	void MutexUnlock();

	void RegisterRigidBody(RigidBodyComponent* rigidBodyComp);
	void UnregisterRigidBody(Object* object);
	void UnregisterRigidBody(RigidBodyComponent* rigidBodyComp);

	void FixedUpdate(const float& fixedDeltaTime);





	bool RaytestSingle(const Ray& ray, float maxDistance, RayHit& hit, FilterGroups group = FilterGroups::EVERYTHING) const;


	static Physics& Instance() // singleton
	{
		static Physics instance;
		return instance;
	}

	Physics(Physics const&) = delete;
	void operator=(Physics const&) = delete;

	//static dx::XMFLOAT3 ToXMFLOAT3(const btVector3& v3) { return dx::XMFLOAT3((float)v3.getX(), (float)v3.getY(), (float)v3.getZ()); }
	//static btVector3 ToVector3(const dx::XMFLOAT3& xm3) { return btVector3(xm3.x, xm3.y, xm3.z); }

private:
	void CheckCollisions();

	CollisionEventListener listener;
	rp::Vector3 gravity;
	rp::PhysicsWorld* world;
	rp::PhysicsCommon common;
	
	std::mutex physicsThreadMutex;
	std::unordered_map<size_t, RigidBodyComponent*> bodyMap;
};