#pragma once
#include <reactphysics3d.h>
#include <DirectXMath.h>
#include "Transform.h"
#include "Component.h"
#include "Object.h"

#include "BoxColliderComponent.h"
#include "SphereColliderComponent.h"
#include "CapsuleColliderComponent.h"
//#include "ChunkCollider.h"

#include <vector>
#include <functional>
#include "CollisionInfo.h"
#include "DShape.h"

namespace dx = DirectX;
namespace rp = reactphysics3d;

enum class ForceMode
{
	FORCE,
	IMPULSE
};

enum class FilterGroups : int;

typedef rp::Quaternion Quaternion;
typedef rp::Vector3 Vector3;
typedef rp::Transform dTransform;
typedef rp::RigidBody RigidBody;
typedef double Scalar;
typedef rp::PhysicsWorld World;

#define STATIC_BODY 0

class RigidBodyComp : public Component
{
public:
	RigidBodyComp(float mass, FilterGroups group, FilterGroups collidesWith);
	virtual ~RigidBodyComp();

	void m_InitializeBody(rp::PhysicsWorld* world);
	RigidBody* GetRigidBody() const { return body; }

	void SetMass(float mass) { this->totalMass = Scalar(mass); }
	float GetMass() const { return static_cast<float>(this->totalMass); }

	virtual void UpdateWorldTransform();
	virtual void m_OnCollision(const CollisionInfo& collision);
	virtual void AddCollisionCallback(std::function<void(CollisionInfo)> callback);

	//virtual void AddForce(const dx::XMFLOAT3& force, const ForceMode& mode);
	//virtual void AddForceAtPoint(const dx::XMFLOAT3& force, const dx::XMFLOAT3& offset, const ForceMode& mode);

	bool IsDynamic() const { return totalMass != 0.0f; }
	FilterGroups GetGroup() const { return this->group; }
	FilterGroups GetCollidesWith() const { return this->collisionMask; }

	void Update(const float& deltaTime) override;

private:
	dTransform ConvertToBtTransform(const Transform& transform) const;
	void AddShapesToCompound(Object* obj, rp::RigidBody* body);

	FilterGroups group;
	FilterGroups collisionMask;

	dTransform bodyTransform;
	RigidBody* body;

	Scalar totalMass;

	std::vector<rp::Collider*> colliders;
	std::vector<std::function<void(CollisionInfo)>> callbacks;
};