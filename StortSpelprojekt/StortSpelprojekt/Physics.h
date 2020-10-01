#pragma once

#include "Bulletphysics\btBulletDynamicsCommon.h"
#include "RigidBodyComp.h"
#include <vector>
#include <iostream>

class Physics
{
public:
	Physics();
	virtual ~Physics();

	void SetGravity(const btVector3& gravity);
	void CreateDynamicWorld();
	void AddRigidBody(RigidBodyComp* rigidBodyComp);
	void FixedUpdate(const float& fixedDeltaTime);
	void SetupShapes();

private:
	void CheckCollisions();

	btVector3 gravity;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	std::vector<RigidBodyComp*> rigidBodyArray;
	std::vector<btVector3> collisions;
};