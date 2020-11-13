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

class ColliderBodyComponent : public Component
{
public:

	void Initialize() override; // OVERRIDE

	BodyType GetType() const { return this->type; }
	FilterGroups GetGroup() const { return this->group; }
	FilterGroups GetCollidesWith() const { return this->collisionMask; }


	void Update(const float& deltaTime) override;

private:

	rp::CollisionBody* body;
	FilterGroups group;
	FilterGroups collisionMask;


	BodyType type;
};