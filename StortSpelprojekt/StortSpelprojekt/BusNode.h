#pragma once
#include <iostream>
#include <functional>
#include "MessageBus.h"

class BusNode
{
public:
	BusNode(MessageBus* messageBus)
	{
		this->messageBus = messageBus;
        this->messageBus->AddReciever(this->getNotifyFunc());
	}

	virtual void Update() {};

    std::function<void(Message)> getNotifyFunc()
    {
        auto messageListener = [=](Message message) -> void {
            this->onNotify(message);
        };
        return messageListener;
    }

    void Send(Message message)
    {
        messageBus->Send(message);
    }

    virtual void onNotify(Message message)
    {
        // Do something here. Your choice. You could do something like this.
        std::cout << "Siopao! Siopao! Siopao! (Someone forgot to implement onNotify().)" << std::endl;
    }

	MessageBus* GetMessageBus() const { return this->messageBus; }

private:
	MessageBus* messageBus;
};



// This class will receive a message from ComponentB.
class ComponentA : public BusNode
{
public:
    ComponentA(MessageBus* messageBus) : BusNode(messageBus) {}

private:
    void onNotify(Message message)
    {
        std::cout << "A: I received: " << message.GetEvent() << std::endl;
    }
};


// This class will send a message to ComponentA.
class ComponentB : public BusNode
{
public:
    ComponentB(MessageBus* messageBus) : BusNode(messageBus) {}

    void Update()
    {
        Message greeting("B SENDING Hi!");
        Send(greeting);
    }

private:
    void onNotify(Message message)
    {
        //std::cout << "B: I received: " << message.GetEvent() << std::endl;
    }
};