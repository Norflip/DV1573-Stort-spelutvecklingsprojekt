#pragma once
#include "Bulletphysics\btBulletDynamicsCommon.h"
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"

#include "RigidBodyComponent.h"
#include <vector>
#include <iostream>
#include "CollisionInfo.h"
#include "Ray.h"
#include <unordered_map>

enum class PhysicsGroup : int
{
	ALL = -1,
	NOTHING = 0,
	DEFAULT = 1 << 0,
	TERRAIN = 1 << 1,
};

DEFINE_ENUM_FLAG_OPERATORS(PhysicsGroup)

struct RayHit
{
	dx::XMFLOAT3 position;
	dx::XMFLOAT3 normal;
	RigidBodyComp* body;
	bool hit;
};

class Physics
{
public:
	Physics();
	virtual ~Physics();

	void Initialize(dx::XMFLOAT3 gravity);

	void SetGravity(float x, float y, float z);
	void CreateDynamicWorld();
	
	// remove rigidbody on chunk and add shape?
	//void ReigsterCollisionObject();

	void RegisterRigidBody(RigidBodyComp* rigidBodyComp);
	void UnregisterRigidBody(Object* object);
	void UnregisterRigidBody(RigidBodyComp* rigidBodyComp);

	void FixedUpdate(const float& fixedDeltaTime);
	
	bool RaytestSingle(const Ray& ray, float maxDistance, RayHit& hit, PhysicsGroup layer = PhysicsGroup::DEFAULT) const;

	static Physics& Instance() // singleton
	{
		static Physics instance;
		return instance;
	}

	Physics(Physics const&) = delete;
	void operator=(Physics const&) = delete;

	static dx::XMFLOAT3 ToXMFLOAT3(const btVector3& v3) { return dx::XMFLOAT3((float)v3.getX(), (float)v3.getY(), (float)v3.getZ()); }
	//static btVector3 ToVector3(const dx::XMFLOAT3& xm3) { return btVector3(xm3.x, xm3.y, xm3.z); }

private:
	void CheckForCollisions();

	btVector3 gravity;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	std::vector<btVector3> collisions;

	std::unordered_map<size_t, RigidBodyComp*> bodyMap;
};