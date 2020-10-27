#pragma once
#include <iostream>
#include <functional>
#include "MessageBus.h"
#include "Log.h"

template <typename T>
class BusNode
{
public:
	BusNode(MessageBus<T>* messageBus)
	{
		this->messageBus = messageBus;
        this->messageBus->AddReciever(GetNotify());
	}

    std::function<void(T)> GetNotify()
    {
        auto messageListener = [=](T message) -> void {
            this->Notify(message);
        };
        return messageListener;
    }

    void Send(T message)
    {
        messageBus->Send(message);
    }

    virtual void Update() {};
    virtual void Notify(T message) {}

	MessageBus<T>* GetMessageBus() const { return this->messageBus; }

private:
	MessageBus<T>* messageBus;
};



// This class will receive a message from ComponentB.
class ComponentA : public BusNode<StrMessage>
{
public:
    ComponentA(MessageBus<StrMessage>* messageBus) : BusNode<StrMessage>(messageBus) {}

private:
    void Notify(StrMessage message)
    {
        Log::Add("A: Received: \"" + message.GetEvent() + "\"");
    }
};


// This class will send a message to ComponentA.
class ComponentB : public BusNode<Message<std::string>>
{
public:
    ComponentB(MessageBus<StrMessage>* messageBus) : BusNode<StrMessage>(messageBus) {}

    void Update()
    {
        Send({ "B sending message" });
    }
};