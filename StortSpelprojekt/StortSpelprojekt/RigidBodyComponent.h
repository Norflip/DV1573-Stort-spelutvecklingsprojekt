#pragma once
#include "Bulletphysics/btBulletDynamicsCommon.h"
#include <DirectXMath.h>
#include "Transform.h"
#include "Component.h"
#include "Object.h"

#include "BoxColliderComponent.h"
#include "SphereColliderComponent.h"
#include "CapsuleColliderComponent.h"

#include <vector>
#include <functional>
#include "CollisionInfo.h"

namespace dx = DirectX;

enum class ForceMode
{
	FORCE,
	IMPULSE
};


#define STATIC_BODY 0

class RigidBodyComp : public Component
{
public:
	RigidBodyComp(float mass);
	virtual ~RigidBodyComp();

	void InitializeBody();
	btRigidBody* GetRigidBody() const { return body; }

	void SetMass(float mass) { this->mass = btScalar(mass); }
	float GetMass() const { return static_cast<float>(this->mass); }

	void m_GenerateCompoundShape(btTransform& transform, btVector3& inertia, btScalar* masses);
	virtual void UpdateWorldTransform(const btDynamicsWorld* world);	
	virtual void m_OnCollision(const CollisionInfo& collision);
	virtual void AddCollisionCallback(std::function<void(CollisionInfo)> callback);

	virtual void AddForce(const dx::XMFLOAT3& force, const ForceMode& mode);
	virtual void AddForceAtPoint(const dx::XMFLOAT3& force, const dx::XMFLOAT3& offset, const ForceMode& mode);

	bool IsDynamic() const { return mass != 0.0f; }

private:
	btTransform ConvertToBtTransform(const Transform& transform) const;
	dx::XMVECTOR ConvertToPosition(const btVector3& position) const;
	dx::XMVECTOR ConvertToRotation(const btQuaternion& rotation) const;
	void RecursiveAddShapes(Object* obj, btCompoundShape* shape);

	btCompoundShape* compShape;
	btRigidBody* body;

	btScalar mass;

	std::vector<std::function<void(CollisionInfo)>> callbacks;
};