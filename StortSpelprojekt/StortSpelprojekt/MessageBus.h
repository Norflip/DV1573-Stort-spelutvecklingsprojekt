#pragma once
#include <functional>
#include <queue>
#include <vector>
#include "Message.h"

class MessageBus
{
public:
	MessageBus() {};
	virtual ~MessageBus() {};

	void AddReciever(std::function<void(Message)> reciever)
	{
		recievers.push_back(reciever);
	}

	void Send (Message message)
	{
		messages.push(message);
	}

	void Notify()
	{
		while (!messages.empty())
		{
			for (auto it = recievers.begin(); it < recievers.end(); it++)
			{
				(*it)(messages.front());
			}

			messages.pop();
		}
	}

private:
	std::vector<std::function<void(Message)>> recievers;
	std::queue<Message> messages;

};