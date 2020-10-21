#pragma once
#include "component.h"
#include "CameraComponent.h"
#include "Object.h"
#include "DXHelper.h"
#include "Transform.h"

namespace dx = DirectX;

class WeaponComponent : public Component
{

public:
	WeaponComponent(Object* object);

	virtual void Initialize() override;
	virtual void FixedUpdate(const float& fixedDeltaTime) override;
	virtual void Update(const float& deltaTime) override;

private:
	Object* camObj;

	dx::XMVECTOR weaponPos;
	dx::XMVECTOR weaponRot;
	dx::XMVECTOR weaponScale;

	dx::XMFLOAT4X4 weaponMatrix;

	void SetPosition();
};