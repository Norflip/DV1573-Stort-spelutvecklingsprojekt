#pragma once
#include "Bulletphysics/btBulletDynamicsCommon.h"
#include <DirectXMath.h>
#include "Transform.h"
#include "Component.h"
#include "Object.h"
#include <vector>

namespace dx = DirectX;

class RigidBodyComp : public Component
{
public:
	RigidBodyComp();
	virtual ~RigidBodyComp();

	void Initialize() override;
	btRigidBody* GetRigidBody();
	void FixedUpdate(const float& fixedDeltaTime) override;
	void UpdateWorldTransform();
	void m_GenerateCompoundShape();

private:
	btBoxShape* CreateBoxShape(const btVector3& halfExtents) const;
	btTransform ConvertToBtTransform(const Transform& transform) const;
	dx::XMVECTOR ConvertToPosition() const;
	dx::XMVECTOR ConvertToRotation() const;
	void RecursiveAddShapes(Object* obj, btCompoundShape* shape);

	btCompoundShape* compShape;
	btRigidBody* body;
	btCollisionShape* colShape;
	btScalar mass;
	btTransform rbTransform;
	btVector3 localInertia;
};