#include "Physics.h"
#include "RigidBodyComponent.h"

Physics::Physics() : world(nullptr)
{
}

Physics::~Physics()
{
	common.destroyPhysicsWorld(world);
	world = nullptr;
}

void Physics::Initialize(dx::XMFLOAT3 gravity)
{
	world = common.createPhysicsWorld();
	// Change the number of iterations of the velocity solver 
	world->setNbIterationsVelocitySolver(15);

	// Change the number of iterations of the position solver 
	world->setNbIterationsPositionSolver(16);
	world->setEventListener(&listener);

	this->gravity = rp::Vector3(gravity.x, gravity.y, gravity.z);
	world->setGravity(this->gravity);

#if LOG_REACT
	// Create the default logger 
	rp::DefaultLogger* logger = common.createDefaultLogger();

	// Log level (warnings and errors) 
	size_t logLevel = static_cast<size_t>(static_cast<size_t>(rp::Logger::Level::Warning) | static_cast<size_t>(rp::Logger::Level::Error));
	
	// Output the logs into an HTML file 
	logger->addFileDestination("Logs/reactphysics_log_latest.txt", logLevel, rp::DefaultLogger::Format::Text);

	// Set the logger 
	common.setLogger(logger);
#endif // LOG_REACT


}

void Physics::MutexLock()
{
	//physicsThreadMutex.lock();
}

void Physics::MutexUnlock()
{
	//physicsThreadMutex.unlock();
}

void Physics::RegisterRigidBody(RigidBodyComponent* rigidBodyComp)
{
	assert(world != nullptr);
	rigidBodyComp->m_InitializeBody(world);
}

void Physics::UnregisterRigidBody(Object* object)
{
	auto find = bodyMap.find(object->GetID());
	if (find != bodyMap.end())
		bodyMap.erase(find);
}

void Physics::UnregisterRigidBody(RigidBodyComponent* rigidBodyComp)
{
	UnregisterRigidBody(rigidBodyComp->GetOwner());
}

void Physics::FixedUpdate(const float& fixedDeltaTime)
{
	MutexLock();

	world->update(fixedDeltaTime);
	CheckCollisions();

	MutexUnlock();
}

bool Physics::RaytestSingle(const Ray& ray, float maxDistance, RayHit& hit, FilterGroups group) const
{
	dx::XMFLOAT3 start = ray.origin;
	dx::XMFLOAT3 end = ray.GetPoint(maxDistance);

	rp::Ray rpray (rp::Vector3(start.x, start.y, start.z), rp::Vector3(end.x, end.y, end.z));
	RaycastSingleResultCallback callback (hit);
	world->raycast(rpray, &callback, static_cast<unsigned short>(group));

	return callback.hit.didHit;
}

void Physics::CheckCollisions()
{
	
}

