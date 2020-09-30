#pragma once
#include "Math.h"
#include "Component.h"
#include <DirectXMath.h>
#include "Object.h"
namespace dx = DirectX;

class PointLightComponent : public Component
{
public:

	PointLightComponent();
	virtual ~PointLightComponent();

	virtual void Update(const float& deltaTime) override;


private:

	dx::XMFLOAT4 lightColor;
	float range;
	dx::XMFLOAT3 attenuation;
	float elapsedTime;

};