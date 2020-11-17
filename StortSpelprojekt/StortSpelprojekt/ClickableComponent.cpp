#include "ClickableComponent.h"

ClickableComponent::ClickableComponent(Type type, float amount)
{
	this->value = amount;
	this->isActive = true;
}

ClickableComponent::~ClickableComponent()
{
}

void ClickableComponent::SetActive(bool isActive)
{
	this->isActive = isActive;
	if (!isActive)
	{
		this->GetOwner()->AddFlag(ObjectFlag::REMOVED);
		this->GetOwner()->RemoveFlag(ObjectFlag::ENABLED);
	}
}

void ClickableComponent::Update(const float& deltaTime)
{
}
