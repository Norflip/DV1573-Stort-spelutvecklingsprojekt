#pragma once
#include <string>

class Message
{
public:
	Message(const std::string message)
	{
		this->message = message;
	}

	std::string GetEvent() const { return this->message; }

private:
	std::string message;
};