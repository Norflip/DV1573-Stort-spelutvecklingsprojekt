#include "RigidBodyComponent.h"
#include "Physics.h"

RigidBodyComp::RigidBodyComp(float mass, FilterGroups group, FilterGroups mask) : totalMass(mass), group(group), collisionMask(collisionMask)
{
}

RigidBodyComp::~RigidBodyComp()
{
}

void RigidBodyComp::Update(const float& deltaTime)
{
	UpdateWorldTransform();

	dx::XMFLOAT3 position;
	dx::XMStoreFloat3(&position, GetOwner()->GetTransform().GetPosition());
	DShape::DrawBox(position, { 1,1,1 }, { 0,0,1 });
}

dTransform RigidBodyComp::ConvertToBtTransform(const Transform& transform) const
{
	dTransform temp;

	dx::XMFLOAT3 tmpPosition;
	dx::XMStoreFloat3(&tmpPosition, transform.GetPosition());
	temp.setPosition(Vector3(tmpPosition.x, tmpPosition.y, tmpPosition.z));

	dx::XMFLOAT4 tmpRotation;
	dx::XMStoreFloat4(&tmpRotation, transform.GetRotation());
	temp.setOrientation(Quaternion(tmpRotation.x, tmpRotation.y, tmpRotation.z, tmpRotation.w));
	return temp;
}

void RigidBodyComp::AddShapesToCompound(Object* obj, rp::RigidBody* body)
{
	const std::vector<Collider*> colliders = obj->GetComponentsOfSubType<Collider>();
	for (size_t i = 0; i < colliders.size(); i++)
	{		
		rp::Collider* collider = body->addCollider(colliders[i]->GetCollisionShape(), colliders[i]->GetTransform());
		//collider->setCollisionCategoryBits(static_cast<int>(group));
		//collider->setCollideWithMaskBits(static_cast<int>(collisionMask));
	}

	std::cout << obj->GetName() << " has colliders: " << colliders.size() << std::endl;

	//CHILDREN
	const std::vector<Transform*>& children = GetOwner()->GetTransform().GetChildren();
	for (size_t i = 0; i < children.size(); i++)
	{
		AddShapesToCompound(children[i]->GetOwner(), body);
	}
}

void RigidBodyComp::m_InitializeBody(rp::PhysicsWorld* world)
{
	Transform& transform = GetOwner()->GetTransform();
	bodyTransform = ConvertToBtTransform(transform);
	btVector3 inertia(0, 0, 0);


	
	rp::BodyType type = rp::BodyType::DYNAMIC;
	if (totalMass == 0.0f)
	{
		type = rp::BodyType::STATIC;
	}
	
	body = world->createRigidBody(bodyTransform);
	body->setType(type);
	body->setMass(totalMass);
	
	AddShapesToCompound(GetOwner(), body);

	//std::cout << GetOwner()->GetName() << " has " << compShape->getNumChildShapes() << " children. " << std::endl;

	//if (IsDynamic())
	//{
	//	int children = compShape->getNumChildShapes();
	//	masses = new btScalar[children];
	//	for (size_t i = 0; i < children; i++)
	//	{
	//		masses[i] = totalMass / children;
	//	}

	//	compShape->calculateLocalInertia(totalMass, inertia);
	//	//compShape->calculatePrincipalAxisTransform(masses, t, inertia);
	//}

	////btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

	//btDefaultMotionState* myMotionState = new btDefaultMotionState(bodyTransform);
	//btRigidBody::btRigidBodyConstructionInfo cInfo(totalMass, myMotionState, compShape, inertia);
	//body = new btRigidBody(cInfo);
	//body->setUserPointer(this);
	//body->setFriction(1);
	////body->setRestitution(0);

	//btVector3 min, max;
	//body->getCollisionShape()->getAabb(bodyTransform, min, max);

	//btVector3 extends = max - min;
	//body->setCcdSweptSphereRadius(extends.m_floats[extends.minAxis()] * 0.5f);
	//body->setCcdMotionThreshold(0.0001f);

	////body->getCollisionShape().
}

void RigidBodyComp::UpdateWorldTransform()
{
	rp::Transform bodyTransform = body->getTransform();



	/*bodyTransform.setIdentity();
	body->getMotionState()->getWorldTransform(bodyTransform);
	*/
	Transform& transform = GetOwner()->GetTransform();
	rp::Vector3 bodyPosition = bodyTransform.getPosition();

	transform.SetPosition(dx::XMVectorSet(
		static_cast <float>(bodyPosition.x),
		static_cast <float>(bodyPosition.y),
		static_cast <float>(bodyPosition.z), 
		0.0f
	));

	rp::Quaternion bodyOrientation = bodyTransform.getOrientation();
	float x = static_cast <float>(bodyOrientation.x);
	float y = static_cast <float>(bodyOrientation.y);
	float z = static_cast <float>(bodyOrientation.z);
	float w = static_cast <float>(bodyOrientation.w);

	transform.SetRotation(dx::XMVectorSet(x, y, z, w));
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

