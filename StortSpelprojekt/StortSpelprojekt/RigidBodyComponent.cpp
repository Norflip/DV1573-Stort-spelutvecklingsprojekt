#include "RigidBodyComponent.h"

RigidBodyComp::RigidBodyComp()
	:colShape(nullptr), compShape(nullptr)
{
}

RigidBodyComp::~RigidBodyComp()
{
}

btTransform RigidBodyComp::ConvertToBtTransform(const Transform& transform) const
{
	btTransform temp;
	dx::XMFLOAT3 tempFloat;
	dx::XMStoreFloat3(&tempFloat, transform.GetPosition());
	temp.setOrigin(btVector3(tempFloat.x, tempFloat.y, tempFloat.z));

	dx::XMStoreFloat3(&tempFloat, transform.GetRotation());
	temp.setRotation(btQuaternion(tempFloat.x, tempFloat.y, tempFloat.z));
	return temp;
}

dx::XMVECTOR RigidBodyComp::ConvertToPosition() const
{
	return { static_cast <float>(rbTransform.getOrigin().getX()), static_cast <float>(rbTransform.getOrigin().getY()), static_cast <float>(rbTransform.getOrigin().getZ()) };
}

dx::XMVECTOR RigidBodyComp::ConvertToRotation() const
{
	return { static_cast <float>(rbTransform.getRotation().getX()), static_cast <float>(rbTransform.getRotation().getY()), static_cast <float>(rbTransform.getRotation().getZ()) };
}

void RigidBodyComp::RecursiveAddShapes(Object* obj, btCompoundShape* shape)
{
}

void RigidBodyComp::Initialize()
{
	Transform& transform = GetOwner()->GetTransform();


	//compShape->addChildShape(rbTransform, colShape);

	rbTransform.setIdentity();

	rbTransform = ConvertToBtTransform(transform);

	mass = 1.0f;

	bool isDynamic = (mass != 0);

	localInertia = btVector3(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(rbTransform);
	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, colShape, localInertia);
	body = new btRigidBody(cInfo);
}

btBoxShape* RigidBodyComp::CreateBoxShape(const btVector3& halfExtents) const
{
	btBoxShape* box = new btBoxShape(halfExtents);
	return box;
}

btRigidBody* RigidBodyComp::GetRigidBody()
{
	return body;
}

void RigidBodyComp::FixedUpdate(const float& fixedDeltaTime)
{
	Transform& transform = GetOwner()->GetTransform();
	transform.SetPosition(ConvertToPosition());
	transform.SetRotation(ConvertToRotation());
}

void RigidBodyComp::UpdateWorldTransform()
{
	body->getMotionState()->getWorldTransform(rbTransform);
}

void RigidBodyComp::m_GenerateCompoundShape()
{
	compShape = new btCompoundShape();
	RecursiveAddShapes(GetOwner(), compShape);
}
