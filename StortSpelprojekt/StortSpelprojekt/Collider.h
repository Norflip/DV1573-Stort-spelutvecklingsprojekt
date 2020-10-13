#pragma once
#include "Bulletphysics/btBulletDynamicsCommon.h"

class Collider
{
public:
	Collider() : shape(nullptr) {};
	virtual ~Collider()
	{
		delete shape;
	}

	//virtual void SetCollisionShape(btCollisionShape* shape) { this->shape = shape; }
		//void SetTransform(btQuaternion rotation, btVector3 translation) { transform = btTransform(rotation, translation); }

	virtual btCollisionShape* GetCollisionShape() const { return this->shape; }
	virtual btTransform GetTransform() const { return this->transform; }

protected:
	
	btTransform transform;
	btCollisionShape* shape;
};