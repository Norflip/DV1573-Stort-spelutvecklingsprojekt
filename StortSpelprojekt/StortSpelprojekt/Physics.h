#pragma once
#include "Bulletphysics\btBulletDynamicsCommon.h"
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"

#include "RigidBodyComponent.h"
#include <vector>
#include <iostream>
#include "CollisionInfo.h"
#include "Ray.h"

class Physics
{
public:
	Physics();
	virtual ~Physics();

	void Initialize(dx::XMFLOAT3 gravity);

	void SetGravity(float x, float y, float z);
	void CreateDynamicWorld();
	void AddRigidBody(RigidBodyComp* rigidBodyComp);
	void FixedUpdate(const float& fixedDeltaTime);
	
	bool RaycastWorld(const Ray& ray, float maxDistance) const;

	//static dx::XMFLOAT3 ToXMFLOAT3(const btVector3& v3) { return dx::XMFLOAT3(v3.x, v3.y, v3.z); }
	//static btVector3 ToVector3(const dx::XMFLOAT3& xm3) { return btVector3(xm3.x, xm3.y, xm3.z); }

private:
	void CheckForCollisions();

	btVector3 gravity;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	std::vector<RigidBodyComp*> rigidBodyArray;
	std::vector<btVector3> collisions;
};