#pragma once

#include "Transform.h"
#include "Component.h"
#include "Object.h"

#include "BoxColliderComponent.h"
#include "SphereColliderComponent.h"
#include "CapsuleColliderComponent.h"
#include "CollisionInfo.h"

namespace dx = DirectX;

//enum class BodyType 
//{
//	STATIC = 0,
//	KINEMATIC = 1,
//	DYNAMIC = 2
//};

enum  class FilterGroups : unsigned short;

enum class BodyType;

class CollisionBodyComponent : public Component
{
public:

	void m_initializeBody(Physics* physics);
	rp::CollisionBody* getcollisionBody() const { return body; }

	void Initialize() override; // OVERRIDE

	BodyType GetType() const { return this->type; }
	FilterGroups GetGroup() const { return this->group; }
	FilterGroups GetCollidesWith() const { return this->collisionMask; }


	void Update(const float& deltaTime) override;

private:

	rp::Transform ConvertToBtTransform(const Transform& transform) const;

	rp::CollisionBody* body;
	Physics* physics;

	FilterGroups group;
	FilterGroups collisionMask;

	rp::Transform currentTransform;
	rp::Transform previousTransform;

	bool initialized;

	BodyType type;
};