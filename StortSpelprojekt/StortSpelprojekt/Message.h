#pragma once

template <typename T>
class Message
{
public:
	Message(const T& msg) : msg(msg) {}
	T GetEvent() const { return this->msg; }

private:
	T msg;
};

typedef Message<std::string> StrMessage;
