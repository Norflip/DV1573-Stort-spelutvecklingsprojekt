#include "Physics.h"

Physics::Physics() : collisionConfiguration(nullptr), dispatcher(nullptr), overlappingPairCache(nullptr)
{
}

Physics::~Physics()
{
}

void Physics::Initialize(dx::XMFLOAT3 gravity)
{
	CreateDynamicWorld();
	SetGravity(gravity.x, gravity.y, gravity.z);
}

void Physics::SetGravity(float x, float y, float z)
{
	this->gravity = btVector3(x, y, z);
    dynamicsWorld->setGravity(this->gravity);
}

void Physics::CreateDynamicWorld()
{
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
}

void Physics::AddRigidBody(RigidBodyComp* rigidBodyComp)
{
	rigidBodyComp->InitializeBody();
	dynamicsWorld->addRigidBody(rigidBodyComp->GetRigidBody(), 1, 1);
	bodies.push_back(rigidBodyComp);
}

void Physics::FixedUpdate(const float& fixedDeltaTime)
{

	// neccesary? 
	dynamicsWorld->updateAabbs();
	dynamicsWorld->computeOverlappingPairs();

	const float internalTimeStep = 1. / 240.f;
	dynamicsWorld->stepSimulation(fixedDeltaTime, 4, internalTimeStep);


	for (int i = bodies.size() - 1; i >= 0; i--)
	{
		bodies[i]->UpdateWorldTransform(dynamicsWorld);
	}
	
	CheckForCollisions();
	/*if (collisions.empty())
	{
		std::printf("0 ");
	}
	else
	{
		std::printf("1 ");
	}*/

}

bool Physics::RaycastWorld(const Ray& ray, float maxDistance) const
{
	dx::XMFLOAT3 furthestPoint = ray.GetPoint(maxDistance);
	btVector3 from = btVector3(ray.origin.x, ray.origin.y, ray.origin.z);
	btVector3 to = btVector3(furthestPoint.x, furthestPoint.y, furthestPoint.z);

	btCollisionWorld::ClosestRayResultCallback closestResults(from, to);
	closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

	dynamicsWorld->rayTest(from, to, closestResults);

	if (closestResults.hasHit())
	{
		btVector3 p = from.lerp(to, closestResults.m_closestHitFraction);
		dynamicsWorld->getDebugDrawer()->drawSphere(p, 0.1, btVector3(0,0,1));
		dynamicsWorld->getDebugDrawer()->drawLine(p, p + closestResults.m_hitNormalWorld, btVector3(0, 0, 1));
		
		void* userPointer = closestResults.m_collisionObject->getUserPointer();
		RigidBodyComp* rb = static_cast<RigidBodyComp*>(userPointer);
		if (rb != nullptr)
		{
			std::cout << "RB!!" << std::endl;
		}
	}

	return false;
}

void Physics::CheckForCollisions()
{
	collisions.clear();
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

		const btCollisionObject* objA = contactManifold->getBody0();
		const btCollisionObject* objB = contactManifold->getBody1();

		CollisionInfo tmp;
		RigidBodyComp* rbA = static_cast<RigidBodyComp*>(objA->getUserPointer());
		RigidBodyComp* rbB = static_cast<RigidBodyComp*>(objB->getUserPointer());

		if (rbA)
		{
			tmp.other = rbB;
			rbA->m_OnCollision(tmp);
		}

		if (rbB)
		{
			tmp.other = rbA;
			rbB->m_OnCollision(tmp);
		}


		/*int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			
			const btVector3& ptA = pt.getPositionWorldOnA();
			const btVector3& ptB = pt.getPositionWorldOnB();
			const btVector3& normalOnB = pt.m_normalWorldOnB;
			collisions.push_back(ptA);
			collisions.push_back(ptB);
			collisions.push_back(normalOnB);
		}*/
	}
}






