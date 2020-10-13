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

	dx::XMFLOAT3 tmpPosition;
	dx::XMStoreFloat3(&tmpPosition, transform.GetPosition());
	temp.setOrigin(btVector3(tmpPosition.x, tmpPosition.y, tmpPosition.z));

	dx::XMFLOAT4 tmpRotation;
	dx::XMStoreFloat4(&tmpRotation, transform.GetRotation());
	temp.setRotation(btQuaternion(tmpRotation.x, tmpRotation.y, tmpRotation.z, tmpRotation.w));
	return temp;
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
	//body->setRestitution(0);

	btVector3 min, max;
	body->getCollisionShape()->getAabb(bodyTransform, min, max);

	btVector3 extends = max - min;
	body->setCcdSweptSphereRadius(extends.m_floats[extends.minAxis()] * 0.5f);
	body->setCcdMotionThreshold(0.0001f);

	//body->getCollisionShape().
}

void RigidBodyComp::UpdateWorldTransform(const btDynamicsWorld* world)
{
	bodyTransform.setIdentity();
	body->getMotionState()->getWorldTransform(bodyTransform);
	Transform& transform = GetOwner()->GetTransform();

	btVector3 btPos = bodyTransform.getOrigin();
	dx::XMVECTOR position = dx::XMVectorSet(static_cast <float>(btPos.getX()), static_cast <float>(btPos.getY()), static_cast <float>(btPos.getZ()), 0.0f);
	transform.SetPosition(position);

	btQuaternion btRotation = bodyTransform.getRotation();
	float x = static_cast <float>(btRotation.getX());
	float y = static_cast <float>(btRotation.getY());
	float z = static_cast <float>(btRotation.getZ());
	float w = static_cast <float>(btRotation.getW());

	transform.SetRotation(dx::XMVectorSet(x, y, z, w));
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
