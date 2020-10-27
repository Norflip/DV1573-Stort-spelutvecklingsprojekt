#pragma once
#include "component.h"
#include "CameraComponent.h"
#include "Object.h"
#include "DXHelper.h"

namespace dx = DirectX;

class WeaponComponent : public Component
{
public:
	WeaponComponent(Object* object);

	virtual void Initialize() override;
	virtual void Update(const float& deltaTime) override;

private:
	Object* camObj;

	dx::XMVECTOR weaponPos;
	dx::XMVECTOR weaponRot;
	dx::XMVECTOR weaponScale;

	dx::XMVECTOR forward = { 0,0,1 };

	dx::XMMATRIX weaponMatrix;

	void SetPosition();
};