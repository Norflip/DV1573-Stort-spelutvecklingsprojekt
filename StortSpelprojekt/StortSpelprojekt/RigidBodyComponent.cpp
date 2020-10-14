#include "RigidBodyComponent.h"
#include "Physics.h"

RigidBodyComp::RigidBodyComp(float mass, FilterGroups group, FilterGroups mask) : totalMass(mass), group(group), collisionMask(mask)
{
}

RigidBodyComp::~RigidBodyComp()
{
}

void RigidBodyComp::Update(const float& deltaTime)
{
	UpdateWorldTransform();
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

void RigidBodyComp::AddCollidersToBody(Object* obj, rp::RigidBody* body)
{
	const std::vector<Collider*> colliders = obj->GetComponentsOfSubType<Collider>();
	for (size_t i = 0; i < colliders.size(); i++)
	{		
		rp::Collider* collider = body->addCollider(colliders[i]->GetCollisionShape(), colliders[i]->GetTransform());
		collider->setCollisionCategoryBits(static_cast<unsigned short>(group));
		collider->setCollideWithMaskBits(static_cast<unsigned short>(collisionMask));
	}

	std::cout << obj->GetName() << " has colliders: " << colliders.size() << std::endl;

	//CHILDREN
	const std::vector<Transform*>& children = GetOwner()->GetTransform().GetChildren();
	for (size_t i = 0; i < children.size(); i++)
	{
		AddCollidersToBody(children[i]->GetOwner(), body);
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
	body->setUserData(static_cast<void*>(GetOwner()));
	
	AddCollidersToBody(GetOwner(), body);
}

void RigidBodyComp::UpdateWorldTransform()
{
	rp::Transform bodyTransform = body->getTransform();

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

