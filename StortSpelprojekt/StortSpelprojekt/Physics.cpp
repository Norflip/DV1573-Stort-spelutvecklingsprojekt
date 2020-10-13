#include "Physics.h"

typedef unsigned int uint;

// Your event listener class 
class YourEventListener : public rp::EventListener {

	// Override the onContact() method 
	virtual void onContact(const rp::CollisionCallback::CallbackData& callbackData) override {

		// For each contact pair 
		for (uint p = 0; p < callbackData.getNbContactPairs(); p++) {

			// Get the contact pair 
			rp::CollisionCallback::ContactPair contactPair = callbackData.getContactPair(p);

			// For each contact point of the contact pair 
			for (uint c = 0; c < contactPair.getNbContactPoints(); c++) {

				// Get the contact point 
				rp::CollisionCallback::ContactPoint contactPoint = contactPair.getContactPoint(c);

				// Get the contact point on the first collider and convert it in world-space 
				rp::Vector3 worldPoint = contactPair.getCollider1()->getLocalToWorldTransform() * contactPoint.getLocalPointOnCollider1();
				std::cout << worldPoint.x << ", " << worldPoint.y << ", " << worldPoint.z << std::endl;
			
			}
		}
	}
};

Physics::Physics() : world(nullptr)
{
}

Physics::~Physics()
{
	int i;

	/*for (i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}

		world->removeCollisionObject(obj);
		delete obj;
	}*/
}

void Physics::Initialize(dx::XMFLOAT3 gravity)
{
	world = common.createPhysicsWorld();
	// Change the number of iterations of the velocity solver 
	world->setNbIterationsVelocitySolver(15);

	// Change the number of iterations of the position solver 
	world->setNbIterationsPositionSolver(8);
	//world->setEventListener(&listener);

	this->gravity = rp::Vector3(gravity.x / 5.0f, gravity.y / 5.0f, gravity.z / 5.0f);
	world->setGravity(this->gravity);

}

void Physics::CreateDynamicWorld()
{
	//btDefaultCollisionConstructionInfo cci;
	//cci.m_defaultMaxPersistentManifoldPoolSize = 80000;
	//cci.m_defaultMaxCollisionAlgorithmPoolSize = 80000;
	//

	//collisionConfiguration = new btDefaultCollisionConfiguration(cci);
	//dispatcher = new btCollisionDispatcher(collisionConfiguration);

	//

	////overlappingPairCache = new btAxisSweep3(btVector3(-1000, -1000, -1000), btVector3(1000, 1000, 1000));
	//overlappingPairCache = new btDbvtBroadphase();


	//solver = new btSequentialImpulseConstraintSolver();
	//world = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	//world->getSolverInfo().m_solverMode = btSolverMode::SOLVER_SIMD || btSolverMode::SOLVER_USE_WARMSTARTING;
	//world->getSolverInfo().m_numIterations = 1;

	// Create a physics world 
	

}

void Physics::MutexLock()
{
	physicsThreadMutex.lock();
}

void Physics::MutexUnlock()
{
	physicsThreadMutex.unlock();
}

void Physics::RegisterRigidBody(RigidBodyComp* rigidBodyComp)
{
	assert(world != nullptr);

	//int group = static_cast<int>(rigidBodyComp->GetGroup());
	//int mask = static_cast<int>(rigidBodyComp->GetMask());
	rigidBodyComp->m_InitializeBody(world);

	//RegisterRigidBody(rigidBodyComp->GetOwner()->GetID(),  rigidBodyComp->GetRigidBody(), group, mask);
	//bodyMap.insert({ rigidBodyComp.GetOwner().GetID(), rigidBodyComp });
}

void Physics::UnregisterRigidBody(Object* object)
{
	auto find = bodyMap.find(object->GetID());
	if (find != bodyMap.end())
		bodyMap.erase(find);
}

void Physics::UnregisterRigidBody(RigidBodyComp* rigidBodyComp)
{
	UnregisterRigidBody(rigidBodyComp->GetOwner());
}

void Physics::FixedUpdate(const float& fixedDeltaTime)
{
	MutexLock();

	// neccesary? 
	//world->updateAabbs();
	//world->computeOverlappingPairs();

	float internalTimeStep = 1. / 240.f;
	//world->stepSimulation(fixedDeltaTime, 4, internalTimeStep);
	world->update(fixedDeltaTime);
	

	//world->stepSimulation(fixedDeltaTime);
	
	CheckForCollisions();
	MutexUnlock();
}

bool Physics::RaytestSingle(const Ray& ray, float maxDistance, RayHit& hit, FilterGroups group) const
{
//	//std::cout << "origin: " << ray.origin.x << ", " << ray.origin.y << ", " << ray.origin.z << std::endl;
//	//std::cout << "direction: " << ray.direction.x << ", " << ray.direction.y << ", " << ray.direction.z << std::endl;
//
//	dx::XMFLOAT3 furthestPoint = ray.GetPoint(maxDistance);
//	btVector3 to = btVector3(furthestPoint.x, furthestPoint.y, furthestPoint.z);
//	btVector3 from = btVector3(ray.origin.x, ray.origin.y, ray.origin.z);
//	
//	//btCollisionWorld::AllHitsRayResultCallback allResults(from, to);
//	//allResults.m_flags |= btTriangleRaycastCallback::kF_KeepUnflippedNormal;
//
//
//	btCollisionWorld::ClosestRayResultCallback closestResults(from, to);
//	closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces | btTriangleRaycastCallback::kF_KeepUnflippedNormal;
//	closestResults.m_collisionFilterGroup = static_cast<int>(group);
//	//closestResults.m_collisionFilterMask = 1;
//
//	world->rayTest(from, to, closestResults);
//	bool didHit = closestResults.hasHit();
//
//	if (didHit)
//	{
//		btVector3 position = from.lerp(to, closestResults.m_closestHitFraction);
//		position = closestResults.m_hitPointWorld;
//
//		btVector3 normal = closestResults.m_hitNormalWorld;
//		//dynamicsWorld->getDebugDrawer()->drawSphere(p, 0.1, btVector3(0,0,1));
//		//dynamicsWorld->getDebugDrawer()->drawLine(p, p + closestResults.m_hitNormalWorld, btVector3(0, 0, 1));
//			
//		void* userPointer = closestResults.m_collisionObject->getUserPointer();
//		hit.body = static_cast<RigidBodyComp*>(userPointer);
//		hit.position = ToXMFLOAT3(position);// { position.getX(), position.getY(), position.getZ() };
//		hit.normal = ToXMFLOAT3(normal);// { normal.getX(), normal.getY(), normal.getZ() };
//	}
//
//	hit.hit = didHit;
	return false;
}

void Physics::CheckForCollisions()
{
//	collisions.clear();
//	int numManifolds = world->getDispatcher()->getNumManifolds();
//	for (int i = 0; i < numManifolds; i++)
//	{
//		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
//
//		const btCollisionObject* colObj0 = contactManifold->getBody0();
//		const btCollisionObject* colObj1 = contactManifold->getBody1();
//
//		int numContacts = contactManifold->getNumContacts();
//		for (int j = 0; j < numContacts; j++)
//		{
//			btManifoldPoint& pt = contactManifold->getContactPoint(j);
//
//			//// one-sided triangles
//			//if (colObj0->getCollisionShape()->getShapeType() == TRIANGLE_SHAPE_PROXYTYPE)
//			//{
//			//	auto triShape = static_cast<const btTriangleShape*>(colObj0->getCollisionShape());
//			//	const btVector3* v = triShape->m_vertices1;
//			//	btVector3 faceNormalLs = btCross(v[1] - v[0], v[2] - v[0]);
//			//	faceNormalLs.normalize();
//			//	btVector3 faceNormalWs = colObj0->getWorldTransform().getBasis() * faceNormalLs;
//			//	float nDotF = btDot(faceNormalWs, pt.m_normalWorldOnB);
//
//			//	if (nDotF <= 0.0f)
//			//	{
//			//		// flip the contact normal to be aligned with the face normal
//			//		pt.m_normalWorldOnB += -2.0f * nDotF * faceNormalWs;
//			//	}
//			//}
//
//
//			if (colObj0->getCollisionShape()->getShapeType() == TRIANGLE_SHAPE_PROXYTYPE)
//			{
//				btVector3 tempNorm;
//				((btTriangleShape*)colObj0->getCollisionShape())->calcNormal(tempNorm);
//				float dot = tempNorm.dot(pt.m_normalWorldOnB);
//				pt.m_normalWorldOnB = dot > 0.0f ? tempNorm : -tempNorm;
//			}
//			else if (colObj1->getCollisionShape()->getShapeType() == TRIANGLE_SHAPE_PROXYTYPE)
//			{
//				btVector3 tempNorm;
//				((btTriangleShape*)colObj1->getCollisionShape())->calcNormal(tempNorm);
//				float dot = tempNorm.dot(pt.m_normalWorldOnB);
//				pt.m_normalWorldOnB = dot > 0.0f ? tempNorm : -tempNorm;
//			}
//		}
//
//
//
//
//		/*CollisionInfo tmp;
//		RigidBodyComp* rbA = static_cast<RigidBodyComp*>(objA->getUserPointer());
//		RigidBodyComp* rbB = static_cast<RigidBodyComp*>(objB->getUserPointer());
//
//		if (rbA)
//		{
//			tmp.other = rbB;
//			rbA->m_OnCollision(tmp);
//		}
//
//		if (rbB)
//		{
//			tmp.other = rbA;
//			rbB->m_OnCollision(tmp);
//		}
//*/
//
//		
//	}
}






