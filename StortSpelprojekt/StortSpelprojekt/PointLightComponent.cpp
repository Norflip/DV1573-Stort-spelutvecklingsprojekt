#include "PointLightComponent.h"

PointLightComponent::PointLightComponent()
{
}

PointLightComponent::~PointLightComponent()
{
}

void PointLightComponent::Update(const float& deltaTime)
{
	this->elapsedTime += deltaTime;

	GetOwner()->GetTransform().GetPosition();
}
