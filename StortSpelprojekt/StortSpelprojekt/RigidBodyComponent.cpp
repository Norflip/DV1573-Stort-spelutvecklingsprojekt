#include "RigidBodyComponent.h"
#include "Physics.h"

RigidBodyComp::RigidBodyComp(float mass, FilterGroups group, FilterGroups mask) : totalMass(mass), group(group), mask(mask), compShape(nullptr), masses(nullptr)
{

}

RigidBodyComp::~RigidBodyComp()
{
}

void RigidBodyComp::Update(const float& deltaTime)
{
	UpdateWorldTransform(nullptr);

	dx::XMFLOAT3 position;
	dx::XMStoreFloat3(&position, GetOwner()->GetTransform().GetPosition());
	DShape::DrawBox(position, { 1,1,1 }, { 0,0,1 });
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

void RigidBodyComp::AddShapesToCompound(Object* obj, btCompoundShape* shape)
{
	const std::vector<Collider*> colliders = obj->GetComponentsOfSubType<Collider>();
	for (size_t i = 0; i < colliders.size(); i++)
	{
		shape->addChildShape(colliders[i]->GetTransform(), colliders[i]->GetCollisionShape());
	}

	std::cout << obj->GetName() << " has colliders: " << colliders.size() << std::endl;

	//CHILDREN
	const std::vector<Transform*>& children = GetOwner()->GetTransform().GetChildren();

	for (size_t i = 0; i < children.size(); i++)
	{
		AddShapesToCompound(children[i]->GetOwner(), shape);
	}
}

void RigidBodyComp::m_InitializeBody()
{
	Transform& transform = GetOwner()->GetTransform();
	bodyTransform = ConvertToBtTransform(transform);
	btVector3 inertia(0, 0, 0);

	compShape = new btCompoundShape();
	AddShapesToCompound(GetOwner(), compShape);

	std::cout << GetOwner()->GetName() << " has " << compShape->getNumChildShapes() << " children. " << std::endl;

	if (IsDynamic())
	{
		int children = compShape->getNumChildShapes();
		masses = new btScalar[children];
		for (size_t i = 0; i < children; i++)
		{
			masses[i] = totalMass / children;
		}

		compShape->calculateLocalInertia(totalMass, inertia);
		//compShape->calculatePrincipalAxisTransform(masses, t, inertia);
	}

	//btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

	btDefaultMotionState* myMotionState = new btDefaultMotionState(bodyTransform);
	btRigidBody::btRigidBodyConstructionInfo cInfo(totalMass, myMotionState, compShape, inertia);
	body = new btRigidBody(cInfo);
	body->setUserPointer(this);
	body->setFriction(1);
	body->setRestitution(0);


}

void RigidBodyComp::UpdateWorldTransform(const btDynamicsWorld* world)
{

	// loopa collision shiet

	//rbTransform.setIdentity();
	body->getMotionState()->getWorldTransform(bodyTransform);

	Transform& transform = GetOwner()->GetTransform();
	transform.SetPosition(ConvertToPosition(bodyTransform.getOrigin()));
	transform.SetRotation(ConvertToRotation(bodyTransform.getRotation()));

	//std::cout << std::to_string(rbTransform.getOrigin().getX()) << ", " << std::to_string(rbTransform.getOrigin().getY()) << ", " << std::to_string(rbTransform.getOrigin().getZ()) << std::endl;

}

void RigidBodyComp::m_GenerateCompoundShape(btTransform& transform, btVector3& inertia, btScalar* masses)
{
	compShape = new btCompoundShape(true, 1);
	AddShapesToCompound(GetOwner(), compShape);

	int children = compShape->getNumChildShapes();
	masses = new btScalar[children];

	for (size_t i = 0; i < children; i++)
	{
		masses[i] = totalMass;
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
