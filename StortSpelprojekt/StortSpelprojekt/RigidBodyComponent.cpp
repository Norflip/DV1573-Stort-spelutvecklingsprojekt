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

	//CHILDREN
	const std::vector<Transform*>& children = GetOwner()->GetTransform().GetChildren();

	for (size_t i = 0; i < children.size(); i++)
	{
		RecursiveAddShapes(children[i]->GetOwner(), shape);
	}
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
		compShape->calculateLocalInertia(mass, localInertia); //this remains null after debugging. Probably one more step before it works

	btDefaultMotionState* myMotionState = new btDefaultMotionState(rbTransform);
	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, compShape, localInertia);
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
