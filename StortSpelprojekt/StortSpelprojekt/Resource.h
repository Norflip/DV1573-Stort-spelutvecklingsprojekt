#pragma once

ALIGN16
class Resource
{
public:
	Resource() {}
	virtual ~Resource() {}

	ALIGN16_ALLOC;
};