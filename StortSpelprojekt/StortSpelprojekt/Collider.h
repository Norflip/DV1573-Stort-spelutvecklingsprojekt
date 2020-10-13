#pragma once
#include <reactphysics3d.h>
namespace rp = reactphysics3d;

class Collider
{
public:
	Collider() : shape(nullptr) {};
	virtual ~Collider()
	{
		//delete shape;
	}

	//virtual void SetCollisionShape(btCollisionShape* shape) { this->shape = shape; }
		//void SetTransform(btQuaternion rotation, btVector3 translation) { transform = btTransform(rotation, translation); }

	virtual rp::CollisionShape* GetCollisionShape() const { return this->shape; }
	virtual rp::Transform GetTransform() const { return this->transform; }

protected:
	rp::Transform transform;
	rp::CollisionShape* shape;
};