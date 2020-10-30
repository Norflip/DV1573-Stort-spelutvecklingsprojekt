#pragma once
#include "Component.h"
#include "Object.h"
#include "Physics.h"
#include "RigidBodyComponent.h"
namespace dx = DirectX;


enum class Type { Health, Fuel, Food };

class PickupComponent:public Component
{
private:
	float value;
	Type pickupType;
	bool isActive;


public:

	PickupComponent(Type type, float amount = 25.f);
	virtual ~PickupComponent();
	void SetActive(bool isActive);
	float GetAmount(){return this->value;}
	Type GetType() { return this->pickupType; }
	void Update(const float& deltaTime);
};
