#pragma once
#include <string>

class Input
{
public:
	Input() {};
	virtual ~Input() {}

	static Input& Instance() // singleton
	{
		static Input instance;
		return instance;
	}

	Input(Input const&) = delete;
	void operator=(Input const&) = delete;

private:

};