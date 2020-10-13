#pragma once
#include "Bulletphysics/btBulletDynamicsCommon.h"
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

enum class ForceMode
{
	FORCE,
	IMPULSE
};

enum class FilterGroups : int;

#define STATIC_BODY 0

class RigidBodyComp : public Component
{
public:
	RigidBodyComp(float mass, FilterGroups group, FilterGroups mask);
	virtual ~RigidBodyComp();

	void m_InitializeBody();
	btRigidBody* GetRigidBody() const { return body; }

	void SetMass(float mass) { this->totalMass = btScalar(mass); }
	float GetMass() const { return static_cast<float>(this->totalMass); }

	void m_GenerateCompoundShape(btTransform& transform, btVector3& inertia, btScalar* masses);
	virtual void UpdateWorldTransform(const btDynamicsWorld* world);	
	virtual void m_OnCollision(const CollisionInfo& collision);
	virtual void AddCollisionCallback(std::function<void(CollisionInfo)> callback);

	virtual void AddForce(const dx::XMFLOAT3& force, const ForceMode& mode);
	virtual void AddForceAtPoint(const dx::XMFLOAT3& force, const dx::XMFLOAT3& offset, const ForceMode& mode);

	bool IsDynamic() const { return totalMass != 0.0f; }
	FilterGroups GetGroup() const { return this->group; }
	FilterGroups GetMask() const { return this->mask; }

	void Update(const float& deltaTime) override;

private:
	btTransform ConvertToBtTransform(const Transform& transform) const;
	void AddShapesToCompound(Object* obj, btCompoundShape* shape);

	FilterGroups group;
	FilterGroups mask;

	btTransform bodyTransform;
	btCompoundShape* compShape;
	btRigidBody* body;

	btScalar totalMass;
	btScalar* masses;

	std::vector<std::function<void(CollisionInfo)>> callbacks;
};