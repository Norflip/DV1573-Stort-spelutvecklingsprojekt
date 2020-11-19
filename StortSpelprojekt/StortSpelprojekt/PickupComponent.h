#pragma once
#include "Component.h"
#include "Object.h"
#include "Physics.h"
#include "RigidBodyComponent.h"
namespace dx = DirectX;


enum class PickupType { Health, Fuel, Food };

class PickupComponent:public Component
{
private:
	float value;
	PickupType pickupType;
	bool isActive;

public:

	PickupComponent(PickupType type, float amount = 25.f);
	virtual ~PickupComponent();
	void SetActive(bool isActive);
	float GetAmount(){return this->value;}
	PickupType GetType() { return this->pickupType; }
	void Update(const float& deltaTime);
};
