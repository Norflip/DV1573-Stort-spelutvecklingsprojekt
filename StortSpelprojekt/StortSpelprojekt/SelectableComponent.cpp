#include "SelectableComponent.h"

SelectableComponent::SelectableComponent()
{
	this->isActive = false;
}

SelectableComponent::~SelectableComponent()
{
	//blah
}

void SelectableComponent::SetActive(bool isActive)
{
	this->isActive = isActive;
}
