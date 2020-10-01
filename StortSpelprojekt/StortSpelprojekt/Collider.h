#pragma once
#include "Physics.h"

class Collider
{

public:

	virtual btCollisionShape* GetCollisionShape()const = 0;

private:

};