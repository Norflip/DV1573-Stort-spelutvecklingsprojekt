#include "RigidBodyComponent.h"

RigidBodyComp::RigidBodyComp(float mass) : mass(mass), compShape(nullptr)
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
	float x = static_cast <float>(rbTransform.getRotation().getX());
	float y = static_cast <float>(rbTransform.getRotation().getY());
	float z = static_cast <float>(rbTransform.getRotation().getZ());
	float w = static_cast <float>(rbTransform.getRotation().getW());

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
	rbTransform.setIdentity();
	rbTransform = ConvertToBtTransform(transform);

	m_GenerateCompoundShape();
	localInertia = btVector3(0, 0, 0);


	compShape->calculateLocalInertia(mass, localInertia); //this remains null after debugging. Probably one more step before it works	

	btCollisionShape* col = new btBoxShape({ 1,1,1 });

	btDefaultMotionState* myMotionState = new btDefaultMotionState(rbTransform);
	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, col, localInertia);
	body = new btRigidBody(cInfo);
	body->setUserPointer(this);
	body->activate(true);

	// register to physics? 
}

void RigidBodyComp::FixedUpdate(const float& fixedDeltaTime)
{
	/*Transform& transform = GetOwner()->GetTransform();
	transform.SetPosition(ConvertToPosition(rbTransform.getOrigin()));
	transform.SetRotation(ConvertToRotation(rbTransform.getRotation()));*/

	std::cout << "fixed" << std::endl;
}

void RigidBodyComp::UpdateWorldTransform(const btDynamicsWorld* world)
{

	std::cout << "update" << std::endl;
	// loopa collision shiet

	rbTransform.setIdentity();
	body->getMotionState()->getWorldTransform(rbTransform);
	
	Transform& transform = GetOwner()->GetTransform();
	transform.SetPosition(ConvertToPosition(rbTransform.getOrigin()));
	transform.SetRotation(ConvertToRotation(rbTransform.getRotation()));

	std::cout << std::to_string(rbTransform.getOrigin().getX()) << ", " << std::to_string(rbTransform.getOrigin().getY()) << ", " << std::to_string(rbTransform.getOrigin().getZ()) << std::endl;

}

void RigidBodyComp::m_GenerateCompoundShape()
{
	compShape = new btCompoundShape(true, 1);
	RecursiveAddShapes(GetOwner(), compShape);

	int children = compShape->getNumChildShapes();

	std::cout << "CHILDREN: " << children << std::endl;

	btScalar* masses = new btScalar[children];
	for (size_t i = 0; i < children; i++)
	{
		masses[i] = mass;
	}

	btVector3 inertia(0, 0, 0);
	btTransform principalTransform;

	compShape->calculatePrincipalAxisTransform(masses, principalTransform, inertia);

	// Transform all the child shapes by the inverse of the compound's principal transform, so
	// as to restore their world positions.
	for (int i = 0; i < compShape->getNumChildShapes(); ++i)
	{
		btCollisionShape* childShape = compShape->getChildShape(i);
		compShape->updateChildTransform(i, principalTransform.inverse() * compShape->getChildTransform(i));
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

void RigidBodyComp::AddForce(const dx::XMFLOAT3& force)
{
	btVector3 forcev3(force.x, force.y, force.z);
	body->applyCentralForce(forcev3);
}
