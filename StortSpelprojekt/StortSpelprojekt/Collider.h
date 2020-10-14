#pragma once
#include "Component.h"
#include <reactphysics3d.h>

namespace rp = reactphysics3d;
namespace dx = DirectX;

class Collider
{
public:
	Collider(dx::XMFLOAT3 position);
	virtual ~Collider() {} 		// delete shape should be done in the physics world

	virtual rp::CollisionShape* GetCollisionShape() const { return this->shape; }
	virtual rp::Transform GetTransform() const { return this->transform; }
	void SetRotation(dx::XMVECTOR quaternion);

protected:
	
	dx::XMFLOAT3 position;
	dx::XMFLOAT4 rotation;
	rp::Transform transform;
	rp::CollisionShape* shape;
};
