#pragma once
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

enum class BodyType
{
	STATIC = 0,
	KINEMATIC = 1,
	DYNAMIC = 2
};

enum class FilterGroups : unsigned short;

#define STATIC_BODY 0

class RigidBodyComponent : public Component
{
public:
	RigidBodyComponent(float mass, FilterGroups group, FilterGroups collidesWith, BodyType type);
	virtual ~RigidBodyComponent();

	void m_InitializeBody(rp::PhysicsWorld* world);
	rp::RigidBody* GetRigidBody() const { return body; }

	void SetMass(float mass) { this->mass = mass; }
	float GetMass() const { return static_cast<float>(this->mass); }

	virtual void m_OnCollision(const CollisionInfo& collision);
	virtual void AddCollisionCallback(std::function<void(CollisionInfo)> callback);

	virtual void AddForce(const dx::XMFLOAT3& force);
	virtual void AddForceAtPoint(const dx::XMFLOAT3& force, const dx::XMFLOAT3& offset, bool local = true);

	virtual void SetLinearVelocity(const dx::XMFLOAT3& velocity);
	virtual void SetLinearDamping(const float damping) { body->setLinearDamping(damping); };
	virtual dx::XMFLOAT3 GetLinearVelocity()const;
	virtual void SetAngularVelocity(const dx::XMFLOAT3& velocity);
	virtual void SetAngularDamping(const float damping) { body->setAngularDamping(damping); };
	virtual dx::XMFLOAT3 GetAngularVelocity()const;
	void EnableGravity(const bool isEnabled) { body->enableGravity(isEnabled); };
	
	BodyType GetType() const { return this->type; }
	FilterGroups GetGroup() const { return this->group; }
	FilterGroups GetCollidesWith() const { return this->collisionMask; }

	void Update(const float& deltaTime) override;
	void SetPosition(dx::XMVECTOR position);
	dx::XMVECTOR GetPosition()const ;
	void SetRotation(dx::XMVECTOR rotation);
	dx::XMVECTOR GetRotation()const ;
	bool IsRotationLocked() const { return this->lockRotation; }
	void LockRotation(bool lock) { this->lockRotation = lock; }

	void PhysicRelease();

	void RemoveCollidersFromBody(rp::RigidBody* body);	// MAYBE HEAAAA? 

private:
	rp::Transform ConvertToBtTransform(const Transform& transform) const;
	void AddCollidersToBody(Object* obj, rp::RigidBody* body);

	std::vector<rp::Collider*> collidersList;
	FilterGroups group;
	FilterGroups collisionMask;

	rp::Transform currentTransform;
	rp::Transform previousTransform;

	rp::RigidBody* body;
	
	float mass;
	bool lockRotation;
	BodyType type;

	std::vector<std::function<void(CollisionInfo)>> callbacks;

};