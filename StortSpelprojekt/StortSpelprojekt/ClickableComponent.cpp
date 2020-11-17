#include "ClickableComponent.h"

ClickableComponent::ClickableComponent(ClickType type)
{
	this->isActive = true;
}

ClickableComponent::~ClickableComponent()
{
	//blah
}

void ClickableComponent::SetActive(bool isActive)
{
	this->isActive = isActive;
}

void ClickableComponent::Update(const float& deltaTime)
{
	//ch
}
