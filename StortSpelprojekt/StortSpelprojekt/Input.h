#pragma once
#include <string>

class Input
{
public:
	Input() {};
	virtual ~Input() {}
	//void Update ();

	std::string Test() { return "TEST"; }

	// SINGLETON
	static Input& Instance()
	{
		static Input instance;
		return instance;
	}

	Input(Input const&) = delete;
	void operator=(Input const&) = delete;

private:

};