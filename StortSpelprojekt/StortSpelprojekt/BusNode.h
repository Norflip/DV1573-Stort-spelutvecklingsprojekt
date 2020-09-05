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


    std::function<void(Message)> getNotifyFunc()
    {
        auto messageListener = [=](Message message) -> void {
            this->OnNotify(message);
        };
        return messageListener;
    }

    void Send(Message message)
    {
        messageBus->Send(message);
    }

    virtual void Update() {};
    virtual void OnNotify(Message message) {}

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
    void OnNotify(Message message)
    {
        Log::Add("A: Received: \"" + message.GetEvent() + "\"");
    }
};


// This class will send a message to ComponentA.
class ComponentB : public BusNode
{
public:
    ComponentB(MessageBus* messageBus) : BusNode(messageBus) {}

    void Update()
    {
        Send({ "B sending message" });
    }
};