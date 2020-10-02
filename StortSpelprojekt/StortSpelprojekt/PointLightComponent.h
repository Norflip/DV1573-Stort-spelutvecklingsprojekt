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

	PointLightComponent(ID3D11Device* device, ID3D11DeviceContext* context);
	PointLightComponent(dx::XMFLOAT4 lightColor, float range , dx::XMFLOAT3 attenuation, ID3D11Device* device, ID3D11DeviceContext* context);
	virtual ~PointLightComponent();

	virtual void Update(const float& deltaTime) override;
	virtual void Initialize() override;
	dx::XMFLOAT4 GetColor();
	void SetColor(dx::XMFLOAT4 color);
	float GetRange();
	void SetRange(float range);
	dx::XMFLOAT3 GetAttenuation();
	void SetAttenuation(dx::XMFLOAT3 attenuation);

private:

	s_PointLight lightData;
	ID3D11Buffer* lightBuffer;
	/*dx::XMFLOAT4 lightColor;
	float range;
	dx::XMFLOAT3 attenuation;*/
	float elapsedTime;

};