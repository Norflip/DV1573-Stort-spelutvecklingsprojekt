#pragma once
#include <functional>
#include <queue>
#include <vector>
#include "Message.h"

template <typename T>
class MessageBus
{
public:
	MessageBus() {};
	virtual ~MessageBus() {};

	void AddReciever(std::function<void(T)> reciever)
	{
		recievers.push_back(reciever);
	}

	void Send (T message)
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
	std::vector<std::function<void(T)>> recievers;
	std::queue<T> messages;

};