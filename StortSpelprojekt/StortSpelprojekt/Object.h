#pragma once
#include <vector>

#include "Transform.h"

class Object
{
public:
	Object();
	virtual ~Object();

	Transform& GetTransform() { return this->transform; }

private:
	Transform transform;

};