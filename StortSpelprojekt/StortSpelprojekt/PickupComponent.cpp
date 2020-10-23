#include "PickupComponent.h"

PickupComponent::PickupComponent(Type type, float amount): pickupType(type)
{	
	this->value = amount;	
	this->isActive = true; 
}

PickupComponent::~PickupComponent()
{
	// nothing new yet
}
void PickupComponent::SetActive(bool isAct)
{
	this->isActive = isAct;
	if (!isActive)
	{
		this->GetOwner()->AddFlag(ObjectFlag::REMOVED);
		this->GetOwner()->RemoveFlag(ObjectFlag::ENABLED);
	}
}

void PickupComponent::Update(const float& deltaTime)
{
	//ch
}