#pragma once
#include "DShape.h"
#include "Component.h"

class DebugBoxShapeComponent : public Component
{
public:
	void Update(const float& deltaTime) override
	{
		dx::XMFLOAT3 position;
		dx::XMStoreFloat3(&position, GetOwner()->GetTransform().GetPosition());
		DShape::DrawBox(position, { 1,1,1 }, { 0,0,1 });
	}
};