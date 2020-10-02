#pragma once
#include "Bulletphysics/btBulletDynamicsCommon.h"

class Collider
{
public:

	virtual btCollisionShape* GetCollisionShape()const = 0;
	virtual btTransform GetTransform()const = 0;

private:
};