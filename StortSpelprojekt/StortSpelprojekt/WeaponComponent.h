#pragma once
#include "component.h"
#include "CameraComponent.h"
#include "Object.h"
#include "DXHelper.h"
#include "Input.h"

namespace dx = DirectX;

ALIGN16 
class WeaponComponent : public Component
{
public:
	WeaponComponent(Object* object);

	virtual void Initialize() override;
	virtual void Update(const float& deltaTime) override;

	ALIGN16_ALLOC;

private:
	Object* camObj;
	CameraComponent* camComp;

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