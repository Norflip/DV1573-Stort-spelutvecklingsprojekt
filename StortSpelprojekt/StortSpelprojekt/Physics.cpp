#include "Physics.h"

Physics::Physics()
    :gravity(NULL, NULL, NULL), collisionConfiguration(nullptr), dispatcher(nullptr), overlappingPairCache(nullptr),
    solver(nullptr), dynamicsWorld(nullptr)
{
}

Physics::~Physics()
{
}

void Physics::SetGravity(const btVector3& gravity)
{
    dynamicsWorld->setGravity(gravity);
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
    dynamicsWorld->addRigidBody(rigidBodyComp->GetRigidBody(), 1, 1);
    rigidBodyArray.push_back(rigidBodyComp);
}

void Physics::FixedUpdate(const float& fixedDeltaTime)
{
    float internalTimeStep = 1. / 240.f;
    dynamicsWorld->stepSimulation(fixedDeltaTime, 4, internalTimeStep);

    for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
    {
        rigidBodyArray[j]->UpdateWorldTransform();
    }

    CheckCollisions();
    if (collisions.empty())
    {
        std::printf("0 ");
    }
    else
    {
        std::printf("1 ");
    }

}

void Physics::SetupShapes()
{
    for (size_t i = 0; i < rigidBodyArray.size(); i++)
    {
        rigidBodyArray[i]->m_GenerateCompoundShape();
    }

}

void Physics::CheckCollisions()
{
    collisions.clear();
    int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
    for (int i = 0; i < numManifolds; i++)
    {
        btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
        {
            const btCollisionObject* objA = contactManifold->getBody0();
            const btCollisionObject* objB = contactManifold->getBody1();
        }
        int numContacts = contactManifold->getNumContacts();
        for (int j = 0; j < numContacts; j++)
        {
            btManifoldPoint& pt = contactManifold->getContactPoint(j);
            const btVector3& ptA = pt.getPositionWorldOnA();
            const btVector3& ptB = pt.getPositionWorldOnB();
            const btVector3& normalOnB = pt.m_normalWorldOnB;
            collisions.push_back(ptA);
            collisions.push_back(ptB);
            collisions.push_back(normalOnB);
        }
    }
}






