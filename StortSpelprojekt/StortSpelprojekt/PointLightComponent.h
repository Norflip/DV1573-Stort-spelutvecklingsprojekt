#pragma once
#include "Math.h"
#include "Component.h"
#include <DirectXMath.h>
#include "Object.h"
#include "Buffers.h"
namespace dx = DirectX;

class PointLightComponent : public Component
{
public:

	PointLightComponent();
	virtual ~PointLightComponent();

	virtual void Initialize() override;
	dx::XMFLOAT4 GetColor();
	void SetColor(dx::XMFLOAT4 color);
	float GetRange();
	void SetRange(float range);
	dx::XMFLOAT3 GetAttenuation();
	void SetAttenuation(dx::XMFLOAT3 attenuation);

	bool IsDirty() { return this->dirty || GetOwner()->GetTransform().ChangedThisFrame(); }
	void MarkAsNotDirty() { this->dirty = false; }

private:
	bool dirty;
	dx::XMFLOAT4 lightColor;
	dx::XMFLOAT3 lightPosition;
	float range;
	dx::XMFLOAT3 attenuation;
	float elapsedTime;

};