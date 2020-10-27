#pragma once
#include "component.h"
#include "CameraComponent.h"
#include "Object.h"
#include "DXHelper.h"
#include "Input.h"

namespace dx = DirectX;

class WeaponComponent : public Component
{
public:
	WeaponComponent(Object* object, Input& input);

	virtual void Initialize() override;
	virtual void Update(const float& deltaTime) override;

private:
	Object* camObj;
	Input* wepInput;

	dx::XMVECTOR weaponPos;
	dx::XMVECTOR weaponRot;
	dx::XMVECTOR weaponScale;
	dx::XMVECTOR up;
	dx::XMMATRIX wepWorld, wepOffTrans, wepOffRot;
	dx::XMMATRIX inverseViewMatrix;


	/// SKA TAS BORT EFTER SPELTEST
	bool attacking;
	float attackTimer = 0;
	float attackCooldown = 0;
	// // // //

	void SetPosition(float time);
};