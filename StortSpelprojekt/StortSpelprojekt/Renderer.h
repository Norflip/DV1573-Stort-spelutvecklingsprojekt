#pragma once
#include "DXHandler.h"

class Renderer
{
public:
	Renderer();
	virtual ~Renderer();

	void Initialize(DXHandler* dxHandler);



private:
	DXHandler* dxhandler;

};