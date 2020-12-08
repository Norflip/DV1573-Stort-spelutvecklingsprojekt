#include "stdafx.h"
#include "Physics.h"
#include "RigidBodyComponent.h"

Physics::Physics() : world(nullptr)
{
}

Physics::~Physics()
{
	//common.destroyPhysicsWorld(world);
	world = nullptr;
}

void Physics::Initialize(dx::XMFLOAT3 gravity)
{
	world = common.createPhysicsWorld();

	// ändra dessa under en viss tid i början?
	world->setNbIterationsVelocitySolver(10); 
	world->setNbIterationsPositionSolver(5);
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

void Physics::FixedUpdate(const float& fixedDeltaTime)
{
	MutexLock();
	world->update(fixedDeltaTime);
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
