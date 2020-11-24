#include "stdafx.h"
#include "PickupComponent.h"

PickupComponent::PickupComponent(PickupType type, float amount): pickupType(type)
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
		this->GetOwner()->RemoveFlag(ObjectFlag::ENABLED);
	else
		this->GetOwner()->AddFlag(ObjectFlag::ENABLED);
}

void PickupComponent::Update(const float& deltaTime)
{
	//ch
}
