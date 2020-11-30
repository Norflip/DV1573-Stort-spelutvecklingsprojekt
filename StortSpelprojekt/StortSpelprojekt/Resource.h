#pragma once



ALIGN16
class Resource
{
	static size_t idCounter;

public:
	Resource();
	virtual ~Resource();
	size_t GetID() const { return this->id; }

	ALIGN16_ALLOC;
private:
	size_t id;

};