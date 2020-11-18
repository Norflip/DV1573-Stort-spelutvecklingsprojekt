#pragma once
#include "Math.h"
#include "Component.h"
#include <DirectXMath.h>
#include "Object.h"
#include "Buffers.h"
namespace dx = DirectX;

constexpr dx::XMFLOAT3 POINT_DEFAULT_ATTENUATION = dx::XMFLOAT3(1.0f, 0.02f, 0.0f);
enum class LightType
{
	POINT_LIGHT = 0,
	SPOT_LIGHT = 1,
	DIRECTIONAL_LIGHT = 2
};
class LightComponent : public Component
{
public:

	LightComponent(UINT type, dx::XMFLOAT4 color, float range, dx::XMFLOAT3 attenuation = POINT_DEFAULT_ATTENUATION);
	virtual ~LightComponent();

	virtual void Initialize() override;
	dx::XMFLOAT4 GetColor();
	void SetColor(dx::XMFLOAT4 color);
	float GetRange();
	void SetRange(float range);
	dx::XMFLOAT3 GetAttenuation();
	void SetAttenuation(dx::XMFLOAT3 attenuation);

	dx::XMFLOAT3 GetDirection();
	void SetDirection(dx::XMFLOAT3 direction);
	float GetSpotlightAngle();
	void SetSpotlightAngle(float angle);
	bool GetEnabled();
	void SetEnabled(bool enabled);
	LightType GetType();
	void SetType(LightType type); //pointlight 0, spotlight 1, directional 2
	float GetIntensity();
	void SetIntensity(float intensity);

	bool IsDirty() { return this->dirty || GetOwner()->GetTransform().ChangedThisFrame(); }
	void MarkAsNotDirty() { this->dirty = false; }

private:
	bool dirty;
	dx::XMFLOAT4 lightColor;
	//dx::XMFLOAT4 lightPosition;
	//(lightPositionVs is calculated every frame)
	dx::XMFLOAT3 lightDirection;
	float range;
	dx::XMFLOAT3 attenuation;
	float spotlightAngle;
	bool enabled;
	LightType type;
	float intensity;
	float elapsedTime;
};