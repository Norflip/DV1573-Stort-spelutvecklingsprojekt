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

class RigidBodyComp : public Component
{
public:
	RigidBodyComp(float mass);
	virtual ~RigidBodyComp();

	void Initialize() override;
	btRigidBody* GetRigidBody() const { return body; }

	void SetMass(float mass) { this->mass = btScalar(mass); }
	float GetMass() const { return static_cast<float>(this->mass); }

	void FixedUpdate(const float& fixedDeltaTime) override;
	void UpdateWorldTransform();
	
	void m_GenerateCompoundShape();
	void m_OnCollision(const CollisionInfo& collision);
	
	void AddCollisionCallback(std::function<void(CollisionInfo)> callback);

private:
	btTransform ConvertToBtTransform(const Transform& transform) const;
	dx::XMVECTOR ConvertToPosition(const btVector3& position) const;
	dx::XMVECTOR ConvertToRotation(const btQuaternion& rotation) const;
	void RecursiveAddShapes(Object* obj, btCompoundShape* shape);

	btCompoundShape* compShape;
	btRigidBody* body;
	btCollisionShape* colShape;
	btScalar mass;
	btTransform rbTransform;
	btVector3 localInertia;

	std::vector<std::function<void(CollisionInfo)>> callbacks;
};