#include "RigidBodyComponent.h"
#include "Physics.h"

RigidBodyComponent::RigidBodyComponent(float mass, FilterGroups group, FilterGroups mask, bool dynamic) : mass(mass), group(group), collisionMask(mask), dynamic(dynamic)
{
}

RigidBodyComponent::~RigidBodyComponent()
{
}

void RigidBodyComponent::Update(const float& deltaTime)
{
	UpdateWorldTransform();
}

void RigidBodyComponent::SetPosition(dx::XMVECTOR position)
{
	dx::XMFLOAT3 pos;
	dx::XMStoreFloat3(&pos, position);

	rp::Transform transform = body->getTransform();
	transform.setPosition(rp::Vector3(pos.x, pos.y, pos.z));
	body->setTransform(transform);
}

rp::Transform RigidBodyComponent::ConvertToBtTransform(const Transform& transform) const
{
	rp::Transform temp;

	dx::XMFLOAT3 tmpPosition;
	dx::XMStoreFloat3(&tmpPosition, transform.GetPosition());
	temp.setPosition(rp::Vector3(tmpPosition.x, tmpPosition.y, tmpPosition.z));

	dx::XMFLOAT4 tmpRotation;
	dx::XMStoreFloat4(&tmpRotation, transform.GetRotation());
	temp.setOrientation(rp::Quaternion(tmpRotation.x, tmpRotation.y, tmpRotation.z, tmpRotation.w));
	return temp;
}

void RigidBodyComponent::AddCollidersToBody(Object* obj, rp::RigidBody* body)
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

void RigidBodyComponent::m_InitializeBody(rp::PhysicsWorld* world)
{
	Transform& transform = GetOwner()->GetTransform();
	bodyTransform = ConvertToBtTransform(transform);
	
	rp::BodyType type = dynamic ? rp::BodyType::DYNAMIC : rp::BodyType::KINEMATIC;
	if (mass == 0.0f)
		type = rp::BodyType::STATIC;
	
	body = world->createRigidBody(bodyTransform);
	body->setType(type);
	body->setMass(mass);
	body->setUserData(static_cast<void*>(GetOwner()));
	
	AddCollidersToBody(GetOwner(), body);
}

void RigidBodyComponent::UpdateWorldTransform()
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

void RigidBodyComponent::m_OnCollision(const CollisionInfo& collision)
{
	for (auto it = callbacks.begin(); it < callbacks.end(); it++)
		(*it)(collision);
}

void RigidBodyComponent::AddCollisionCallback(std::function<void(CollisionInfo)> callback)
{
	callbacks.push_back(callback);
}

void RigidBodyComponent::AddForce(const dx::XMFLOAT3& force)
{
	rp::Vector3 rpForce(force.x, force.y, force.z);
	body->applyForceToCenterOfMass(rpForce);
}

void RigidBodyComponent::AddForceAtPoint(const dx::XMFLOAT3& force, const dx::XMFLOAT3& offset, bool local)
{
	rp::Vector3 rpForce(force.x, force.y, force.z);
	rp::Vector3 rpOffset(offset.x, offset.y, offset.z);

	if (local)
		body->applyForceAtLocalPosition(rpForce, rpOffset);
	else
		body->applyForceAtWorldPosition(rpForce, rpOffset);
}

