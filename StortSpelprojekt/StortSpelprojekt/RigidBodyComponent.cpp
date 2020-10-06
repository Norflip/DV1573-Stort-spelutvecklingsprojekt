#include "RigidBodyComponent.h"

RigidBodyComp::RigidBodyComp(float mass) : mass(mass),compShape(nullptr)
{

}

RigidBodyComp::~RigidBodyComp()
{
}

btTransform RigidBodyComp::ConvertToBtTransform(const Transform& transform) const
{
	btTransform temp;
	temp.setIdentity();

	dx::XMFLOAT3 tempFloat;
	dx::XMStoreFloat3(&tempFloat, transform.GetPosition());
	temp.setOrigin(btVector3(tempFloat.x, tempFloat.y, tempFloat.z));

	dx::XMStoreFloat3(&tempFloat, transform.GetRotation());
	temp.setRotation(btQuaternion(tempFloat.x, tempFloat.y, tempFloat.z));
	return temp;
}

dx::XMVECTOR RigidBodyComp::ConvertToPosition(const btVector3& position) const
{
	return { static_cast <float>(position.getX()), static_cast <float>(position.getY()), static_cast <float>(position.getZ()) };
}

dx::XMVECTOR RigidBodyComp::ConvertToRotation(const btQuaternion& rotation) const
{
	float x = static_cast <float>(rotation.getX());
	float y = static_cast <float>(rotation.getY());
	float z = static_cast <float>(rotation.getZ());
	float w = static_cast <float>(rotation.getW());

	return { x,y,z,w };
}

void RigidBodyComp::RecursiveAddShapes(Object* obj, btCompoundShape* shape)
{
	//BOXES
	const std::vector<BoxColliderComponent*>& boxes = obj->GetComponents<BoxColliderComponent>();
	for (size_t i = 0; i < boxes.size(); i++)
	{
		shape->addChildShape(boxes[i]->GetTransform(), boxes[i]->GetCollisionShape());
	}

	//SPHERES
	const std::vector<SphereColliderComponent*>& spheres = obj->GetComponents<SphereColliderComponent>();
	for (size_t i = 0; i < spheres.size(); i++)
	{
		shape->addChildShape(spheres[i]->GetTransform(), spheres[i]->GetCollisionShape());
	}

	//CAPSULES
	const std::vector<CapsuleColliderComponent*>& capsules = obj->GetComponents<CapsuleColliderComponent>();
	for (size_t i = 0; i < capsules.size(); i++)
	{
		shape->addChildShape(capsules[i]->GetTransform(), capsules[i]->GetCollisionShape());
	}



	////CHILDREN
	//const std::vector<Transform*>& children = GetOwner()->GetTransform().GetChildren();

	//for (size_t i = 0; i < children.size(); i++)
	//{
	//	RecursiveAddShapes(children[i]->GetOwner(), shape);
	//}
}

void RigidBodyComp::InitializeBody()
{
	Transform& transform = GetOwner()->GetTransform();
	btTransform rbTransform = ConvertToBtTransform(transform);
	btVector3 inertia(0, 0, 0);

	compShape = new btCompoundShape();
	RecursiveAddShapes(GetOwner(), compShape);

	btTransform t;
	t.setIdentity();
	t.setOrigin({ 0,0,0 });

	if (IsDynamic())
	{
		int children = compShape->getNumChildShapes();
		btScalar* masses = new btScalar[children];
		for (size_t i = 0; i < children; i++)
		{
			masses[i] = mass / children;
		}

		compShape->calculatePrincipalAxisTransform(masses, t, inertia);
	}

	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

	btDefaultMotionState* myMotionState = new btDefaultMotionState(rbTransform);
	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, groundShape, inertia);
	body = new btRigidBody(cInfo);
	body->setUserPointer(this);
	body->setFriction(1);
	body->setRestitution(0);
}

void RigidBodyComp::UpdateWorldTransform(const btDynamicsWorld* world)
{

	// loopa collision shiet

	//rbTransform.setIdentity();
	btTransform rbTransform;
	body->getMotionState()->getWorldTransform(rbTransform);

	Transform& transform = GetOwner()->GetTransform();
	transform.SetPosition(ConvertToPosition(rbTransform.getOrigin()));
	transform.SetRotation(ConvertToRotation(rbTransform.getRotation()));

	//std::cout << std::to_string(rbTransform.getOrigin().getX()) << ", " << std::to_string(rbTransform.getOrigin().getY()) << ", " << std::to_string(rbTransform.getOrigin().getZ()) << std::endl;

}

void RigidBodyComp::m_GenerateCompoundShape(btTransform& transform, btVector3& inertia, btScalar* masses)
{
	compShape = new btCompoundShape(true, 1);
	RecursiveAddShapes(GetOwner(), compShape);

	int children = compShape->getNumChildShapes();

	std::cout << "CHILDREN: " << children << std::endl;

	masses = new btScalar[children];
	for (size_t i = 0; i < children; i++)
	{
		masses[i] = mass;
	}
}

void RigidBodyComp::m_OnCollision(const CollisionInfo& collision)
{
	for (auto it = callbacks.begin(); it < callbacks.end(); it++)
		(*it)(collision);
}

void RigidBodyComp::AddCollisionCallback(std::function<void(CollisionInfo)> callback)
{
	callbacks.push_back(callback);
}

void RigidBodyComp::AddForce(const dx::XMFLOAT3& force, const ForceMode& mode)
{
	btVector3 forcev3(force.x, force.y, force.z);

	switch (mode)
	{
		case ForceMode::IMPULSE:
			body->applyCentralImpulse(forcev3);

		case ForceMode::FORCE:
		default:
			body->applyCentralForce(forcev3);
			break;
	}
}

void RigidBodyComp::AddForceAtPoint(const dx::XMFLOAT3& force, const dx::XMFLOAT3& offset, const ForceMode& mode)
{
	btVector3 forcev3(force.x, force.y, force.z);
	btVector3 pos(offset.x, offset.y, offset.z);

	switch (mode)
	{
		case ForceMode::IMPULSE:
			body->applyImpulse(forcev3, pos);

		case ForceMode::FORCE:
		default:
			body->applyForce(forcev3, pos);
			break;
	}
}
