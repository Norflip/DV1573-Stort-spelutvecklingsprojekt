#pragma once
#include "Texture.h"

class RenderPass
{
public:
	RenderPass(int priority);
	virtual ~RenderPass();

	virtual void Pass() = 0;
	int GetPriority() const { return this->priority; }

private:
	int priority;

};